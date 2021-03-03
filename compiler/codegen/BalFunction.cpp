#include "BalFunction.h"
#include "BalBasicBlock.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"

namespace nballerina {

Function::Function(Package *_parentPackage, std::string _name,
                   std::string _workerName, int _flags, InvokableType *_type)
    : parentPackage(_parentPackage), name(_name), workerName(_workerName),
      flags(_flags), type(_type) {
  restParam = nullptr;
  receiver = nullptr;
}

// Search basic block based on the basic block ID
BasicBlock *Function::searchBb(std::string name) {
  std::vector<BasicBlock *>::iterator itr;
  for (itr = basicBlocks.begin(); itr != basicBlocks.end(); itr++) {
    if ((*itr)->getId() == name) {
      return (*itr);
    }
  }
  return NULL;
}

std::string Function::getName() { return name; }
FunctionParam *Function::getParam(int i) { return requiredParams[i]; }
RestParam *Function::getRestParam() { return restParam; }
Variable *Function::getReturnVar() { return returnVar; }
std::vector<BasicBlock *> Function::getBasicBlocks() { return basicBlocks; }
LLVMBuilderRef Function::getLLVMBuilder() { return llvmBuilder; }
LLVMValueRef Function::getLLVMFunctionValue() { return llvmFunction; }

LLVMValueRef Function::getLLVMValueForBranchComparison(std::string lhsName) {
  auto branch = branchComparisonList.find(lhsName);
  if (branch == branchComparisonList.end())
    return nullptr;

  return branch->second;
}

LLVMValueRef Function::getLocalVarRefUsingId(std::string locVar) {
  auto varIt = localVarRefs.find(locVar);
  if (varIt == localVarRefs.end())
    return nullptr;

  return varIt->second;
}

LLVMValueRef Function::getLocalToTempVar(Operand *operand) {
  std::string refOp = operand->getName();
  std::string tempName = refOp + "_temp";
  LLVMValueRef locVRef = getLocalVarRefUsingId(refOp);
  if (!locVRef)
    locVRef = parentPackage->getGlobalVarRefUsingId(refOp);
  return LLVMBuildLoad(llvmBuilder, locVRef, tempName.c_str());
}

static bool isParamter(Variable *locVar) {
  switch (locVar->getKind()) {
  case LOCAL_VAR_KIND:
  case TEMP_VAR_KIND:
  case RETURN_VAR_KIND:
  case GLOBAL_VAR_KIND:
  case SELF_VAR_KIND:
  case CONSTANT_VAR_KIND:
    return false;
  case ARG_VAR_KIND:
    return true;
  default:
    return false;
  }
}

LLVMTypeRef Function::getLLVMFuncRetTypeRefOfType(Variable *vDecl) {
  TypeTag typeTag = vDecl->getTypeDecl()->getTypeTag();
  // if main function return type is void, but user wants to return some
  // value using _bal_result (global variable from BIR), change main function
  // return type from void to global variable (_bal_result) type.
  if (typeTag == TYPE_TAG_NIL || typeTag == TYPE_TAG_VOID) {
    Variable *globRetVar =
        parentPackage->getGlobalVarDeclFromName("_bal_result");
    if (globRetVar)
      typeTag = globRetVar->getTypeDecl()->getTypeTag();
  }

  switch (typeTag) {
  case TYPE_TAG_INT:
    return LLVMInt32Type();
  case TYPE_TAG_BYTE:
  case TYPE_TAG_FLOAT:
    return LLVMFloatType();
  case TYPE_TAG_BOOLEAN:
    return LLVMInt8Type();
  case TYPE_TAG_CHAR_STRING:
  case TYPE_TAG_STRING:
  case TYPE_TAG_MAP:
    return LLVMPointerType(LLVMInt8Type(), 0);
  default:
    return LLVMVoidType();
  }
}

void Function::translateFunctionBody(LLVMModuleRef &modRef) {
  LLVMBasicBlockRef BbRef;
  int paramIndex = 0;
  BbRef = LLVMAppendBasicBlock(llvmFunction, "entry");
  LLVMPositionBuilderAtEnd(llvmBuilder, BbRef);

  // iterate through all local vars.
  for (auto const &it : localVars) {
    Variable *locVar = it.second;
    LLVMTypeRef varType =
        parentPackage->getLLVMTypeRefOfType(locVar->getTypeDecl());
    LLVMValueRef localVarRef;
    if (locVar->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {
      varType = wrap(parentPackage->getStructType());
    }
    localVarRef =
        LLVMBuildAlloca(llvmBuilder, varType, (locVar->getName()).c_str());
    localVarRefs.insert({locVar->getName(), localVarRef});
    if (isParamter(locVar)) {
      LLVMValueRef parmRef = LLVMGetParam(llvmFunction, paramIndex);
      std::string paramName = getParam(paramIndex)->getName();
      LLVMSetValueName2(parmRef, paramName.c_str(), paramName.length());
      if (parmRef)
        LLVMBuildStore(llvmBuilder, parmRef, localVarRef);
      paramIndex++;
    }
  }

  // iterate through with each basic block in the function and create them
  // first.
  for (unsigned int i = 0; i < basicBlocks.size(); i++) {
    BasicBlock *bb = basicBlocks[i];
    LLVMBasicBlockRef bbRef =
        LLVMAppendBasicBlock(llvmFunction, bb->getId().c_str());
    bb->setLLVMBBRef(bbRef);
  }

  // creating branch to next basic block.
  if (basicBlocks.size() != 0 && basicBlocks[0] &&
      basicBlocks[0]->getLLVMBBRef())
    LLVMBuildBr(llvmBuilder, basicBlocks[0]->getLLVMBBRef());

  // Now translate the basic blocks (essentially add the instructions in them)
  for (unsigned int i = 0; i < basicBlocks.size(); i++) {
    BasicBlock *bb = basicBlocks[i];
    LLVMPositionBuilderAtEnd(llvmBuilder, bb->getLLVMBBRef());
    bb->translate(modRef);
  }
}

void Function::translate(LLVMModuleRef &modRef) {
  translateFunctionBody(modRef);
}

void Function::insertParam(FunctionParam *param) {
  requiredParams.push_back(param);
}
void Function::setReceiver(Variable *var) { receiver = var; }
void Function::setRestParam(RestParam *param) { restParam = param; }
void Function::insertLocalVar(Variable *var) {
  localVars.insert(std::pair<std::string, Variable *>(var->getName(), var));
}
void Function::setReturnVar(Variable *var) { returnVar = var; }
void Function::insertBasicBlock(BasicBlock *bb) { basicBlocks.push_back(bb); }
void Function::setLLVMBuilder(LLVMBuilderRef b) { llvmBuilder = b; }
void Function::setLLVMFunctionValue(LLVMValueRef newFuncRef) {
  llvmFunction = newFuncRef;
}
void Function::insertBranchComparisonValue(std::string name,
                                           LLVMValueRef compRef) {
  branchComparisonList.insert(
      std::pair<std::string, LLVMValueRef>(name, compRef));
}

Variable *Function::getLocalVarFromName(std::string opName) {

  auto varIt = localVars.find(opName);
  if (varIt == localVars.end())
    return nullptr;

  return varIt->second;
}

Variable *Function::getLocalOrGlobalVariable(Operand *op) {
  if (op->getKind() == GLOBAL_VAR_KIND)
    return parentPackage->getGlobalVarDeclFromName(op->getName());
  return getLocalVarFromName(op->getName());
}

LLVMValueRef Function::getLocalOrGlobalLLVMValue(Operand *op) {
  if (op->getKind() == GLOBAL_VAR_KIND)
    return parentPackage->getGlobalVarRefUsingId(op->getName());
  return getLocalVarRefUsingId(op->getName());
}

Package *Function::getPackage() { return parentPackage; }
size_t Function::getNumParams() { return requiredParams.size(); }

} // namespace nballerina
