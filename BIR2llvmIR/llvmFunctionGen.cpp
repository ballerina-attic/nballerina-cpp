#include "BIR.h"

BIRFunction::BIRFunction() {}

BIRFunction::BIRFunction(Location *pos, string namep, int flagsp,
                         InvokableType *typep, string workerNamep)
    : BIRNode(pos), name(namep), flags(flagsp), type(typep),
      workerName(workerNamep) {}

BIRFunction::BIRFunction(const BIRFunction &) {}

// Search basic block based on the basic block ID
BIRBasicBlock *BIRFunction::searchBb(std::string name) {
  std::vector<BIRBasicBlock *>::iterator itr;
  for (itr = basicBlocks.begin(); itr != basicBlocks.end(); itr++) {
    if ((*itr)->getId() == name) {
      return (*itr);
    }
  }
  return NULL;
}

LLVMValueRef BIRFunction::getValueRefBasedOnName(string lhsName) {
  map<string, LLVMValueRef>::iterator it;
  it = branchComparisonList.find(lhsName);

  if (it == branchComparisonList.end()) {
    return NULL;
  } else
    return it->second;
}

LLVMValueRef BIRFunction::getLocalVarRefUsingId(string locVar) {
  for (std::map<string, LLVMValueRef>::iterator iter = localVarRefs.begin();
       iter != localVarRefs.end(); iter++) {
    if (iter->first == locVar)
      return iter->second;
  }
  return NULL;
}

LLVMValueRef BIRFunction::getLocalToTempVar(Operand *operand) {
  string refOp = operand->getVarDecl()->getVarName();
  string tempName = refOp + "_temp";
  LLVMValueRef locVRef = getLocalVarRefUsingId(refOp);
  return LLVMBuildLoad(builder, locVRef, tempName.c_str());
}

static bool isParamter(VarDecl *locVar) {
  switch (locVar->getVarKind()) {
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

LLVMTypeRef BIRFunction::getLLVMFuncRetTypeRefOfType(VarDecl *vDecl) {
  int typeTag = 0;
  if (vDecl->getTypeDecl())
    typeTag = vDecl->getTypeDecl()->getTypeTag();
  // if main function return type is void, but user wants to return some
  // value using _bal_result (global variable from BIR), change main function
  // return type from void to global variable (_bal_result) type.
  if (typeTag == TYPE_TAG_NIL || typeTag == TYPE_TAG_VOID) {
    vector<VarDecl *> globVars = getPkgAddress()->getGlobalVars();
    for (unsigned int i = 0; i < globVars.size(); i++) {
      VarDecl *globVar = globVars[i];
      if (globVar->getVarName() == "_bal_result") {
        typeTag = globVar->getTypeDecl()->getTypeTag();
        break;
      }
    }
  }

  switch (typeTag) {
  case TYPE_TAG_INT:
    return LLVMInt32Type();
  case TYPE_TAG_BYTE:
  case TYPE_TAG_FLOAT:
  case TYPE_TAG_BOOLEAN:
    return LLVMInt8Type();
  default:
    return LLVMVoidType();
  }
}

void BIRFunction::translateFunctionBody(LLVMModuleRef &modRef) {
  LLVMBasicBlockRef BbRef;
  int paramIndex = 0;
  BbRef = LLVMAppendBasicBlock(newFunction, "entry");
  LLVMPositionBuilderAtEnd(builder, BbRef);

  // iterate through all local vars.
  for (unsigned int i = 0; i < localVars.size(); i++) {
    VarDecl *locVar = localVars[i];
    const char *varName = (locVar->getVarName()).c_str();
    LLVMTypeRef varType = getLLVMTypeRefOfType(locVar->getTypeDecl());
    LLVMValueRef localVarRef;
    if (locVar->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {
      varType = wrap(getPkgAddress()->getStructType());
    }
    localVarRef = LLVMBuildAlloca(builder, varType, varName);
    localVarRefs.insert({locVar->getVarName(), localVarRef});
    if (isParamter(locVar)) {
      LLVMValueRef parmRef = LLVMGetParam(newFunction, paramIndex);
      string paramName = getParam(paramIndex)->getVarDecl()->getVarName();
      LLVMSetValueName2(parmRef, paramName.c_str(), paramName.length());
      if (parmRef)
        LLVMBuildStore(builder, parmRef, localVarRef);
      paramIndex++;
    }
  }

  // iterate through with each basic block in the function and create them
  // first.
  for (unsigned int i = 0; i < basicBlocks.size(); i++) {
    BIRBasicBlock *bb = basicBlocks[i];
    LLVMBasicBlockRef bbRef =
        LLVMAppendBasicBlock(this->getNewFunctionRef(), bb->getId().c_str());
    bb->setLLVMBBRef(bbRef);
    bb->setBIRFunction(this);
    bb->setLLVMBuilderRef(builder);
    bb->setPkgAddress(getPkgAddress());
  }

  // creating branch to next basic block.
  if (basicBlocks.size() != 0 && basicBlocks[0] && basicBlocks[0]->getLLVMBBRef())
    LLVMBuildBr(builder, basicBlocks[0]->getLLVMBBRef());

  // Now translate the basic blocks (essentially add the instructions in them)
  for (unsigned int i = 0; i < basicBlocks.size(); i++) {
    BIRBasicBlock *bb = basicBlocks[i];
    LLVMPositionBuilderAtEnd(builder, bb->getLLVMBBRef());
    bb->translate(modRef);
  }
}

void BIRFunction::translate(LLVMModuleRef &modRef) {
  translateFunctionBody(modRef);
}

BIRFunction::~BIRFunction() {}

LLVMTypeRef BIRFunction::getLLVMTypeRefOfType(TypeDecl *typeD) {
  int typeTag = typeD->getTypeTag();
  switch (typeTag) {
  case TYPE_TAG_INT:
    return LLVMInt32Type();
  case TYPE_TAG_BYTE:
  case TYPE_TAG_FLOAT:
  case TYPE_TAG_BOOLEAN:
    return LLVMInt8Type();
  case TYPE_TAG_ANY:
    return LLVMPointerType(LLVMInt64Type(),0);
  default:
    return LLVMInt32Type();
  }
}

VarDecl *BIRFunction::getNameVarDecl(string opName) {
  for (unsigned int i = 0; i < localVars.size(); i++) {
    VarDecl *variableDecl = localVars[i];
    string varName = variableDecl->getVarName();
    if (varName == opName) {
      return variableDecl;
    }
  }
  return NULL;
}
