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
  if(!locVRef)
    locVRef = getPkgAddress()->getGlobalVarRefUsingId(refOp);
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
    return LLVMFloatType();
  case TYPE_TAG_BOOLEAN:
    return LLVMInt8Type();
  case TYPE_TAG_CHAR_STRING:
  case TYPE_TAG_STRING:
    return LLVMPointerType(LLVMInt8Type(), 0);
  default:
    return LLVMVoidType();
  }
}

LLVMValueRef BIRFunction::generateAbortInsn (LLVMBuilderRef &builder,
                LLVMModuleRef &modRef) {
  BIRPackage *pkgObj  = getPkgAddress();
  LLVMValueRef addedFuncRef = pkgObj->getFunctionRefBasedOnName ("abort");
  LLVMValueRef *paramRefs = new LLVMValueRef[0];
  if (!addedFuncRef) {
    LLVMTypeRef *paramTypes = new LLVMTypeRef[0];
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 0, 0);
    addedFuncRef = LLVMAddFunction(modRef, "abort", funcType);
    pkgObj->addArrayFunctionRef("abort", addedFuncRef);
  }
  return LLVMBuildCall(builder, addedFuncRef, paramRefs, 0, "");
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

  // Splitting Basicblock after 'is_same_type()' function call and 
  // based on is_same_type() function result, crating branch condition using
  // New Basicblock (ifBB) and abortBB(elseBB).
  // In IfBB we are doing casing and from ElseBB Aborting.

  unsigned totalBB = LLVMCountBasicBlocks(getNewFunctionRef());
  LLVMBasicBlockRef currentBB = LLVMGetFirstBasicBlock(getNewFunctionRef());;
  for (unsigned i = 0; i < totalBB; i++) {
    BasicBlock *Block = unwrap(currentBB);
    for (BasicBlock::iterator I = Block->begin(); I != Block->end(); ++I) {
      CallInst *CI = dyn_cast<CallInst>(&*I);
      if (CI) {
	string insnName;
	if (CI->getNumOperands() == 2)
	  insnName = CI->getOperand(1)->getName().data();
	else if (CI->getNumOperands() == 3)
	  insnName = CI->getOperand(2)->getName().data();
	if (insnName == "is_same_type") {
	  advance(I,1);
	  Instruction  *compInsn = &*I;
	  // Splitting BasicBlock.
	  BasicBlock *New = Block->splitBasicBlock(
	     ++I, Block->getName() + ".split");
	  LLVMValueRef lastInsn = LLVMGetLastInstruction(wrap(Block));
	  // branch intruction to the split BB is creating in BB2 (last BB) basicblock,
	  // removing from BB2 and insert this branch instruction into BB0(split original BB).
	  LLVMInstructionRemoveFromParent(lastInsn);
	  // Creating abortBB (elseBB).
          LLVMBasicBlockRef elseLLVMBB = LLVMCreateBasicBlockInContext(
			  LLVMGetGlobalContext(), "abortBB");
	  // Creating Branch condition using if and else BB's.
	  LLVMValueRef Comp = LLVMBuildCondBr(builder, wrap(compInsn), wrap(New), elseLLVMBB);
	  // branch to abortBB instruction is generating in last(e.g bb2 BB) basicblock.
	  // here, moving from bb2 to bb0.split basicblock.
	  LLVMInstructionRemoveFromParent(Comp);
          Block->getInstList().push_back(unwrap<Instruction>(Comp));
	  LLVMValueRef newBBLastInsn = LLVMGetLastInstruction(wrap(New));
          LLVMBasicBlockRef elseBBSucc = LLVMGetSuccessor(newBBLastInsn, 0);
	  // creating branch to else basicblock.
	  LLVMValueRef brInsn = LLVMBuildBr(builder, elseBBSucc);
	  LLVMInstructionRemoveFromParent(brInsn);
	  // generate LLVMFunction call to Abort from elseLLVMBB(abortBB).
	  LLVMValueRef abortInsn = generateAbortInsn (builder, modRef);
	  LLVMInstructionRemoveFromParent(abortInsn);
	  // Inserting Abort Functioncall instruction into elseLLVMBB(abortBB).
	  unwrap(elseLLVMBB)->getInstList().push_back(unwrap<Instruction>(abortInsn));
	  unwrap(elseLLVMBB)->getInstList().push_back(unwrap<Instruction>(brInsn));
	  // Inserting elseLLVMBB (abort BB) after New (bb0.split) basicblock.
	  New->getParent()->getBasicBlockList().insertAfter(New->getIterator(),
                                                      unwrap(elseLLVMBB));
	  currentBB = LLVMGetNextBasicBlock(currentBB);
	  break;
	}
      }
    }
    currentBB = LLVMGetNextBasicBlock(currentBB);
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
    return LLVMFloatType();
  case TYPE_TAG_BOOLEAN:
    return LLVMInt8Type();
  case TYPE_TAG_CHAR_STRING:
  case TYPE_TAG_STRING:
    return LLVMPointerType(LLVMInt8Type(), 0);
  case TYPE_TAG_ANY:
    return LLVMPointerType(LLVMInt64Type(), 0);
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

const char *BIRFunction::getTypeNameOfTypeTag(TypeTagEnum typeTag) {
  switch (typeTag) {
  case TYPE_TAG_INT:
    return "int";
  case TYPE_TAG_FLOAT:
    return "float";
  case TYPE_TAG_CHAR_STRING:
  case TYPE_TAG_STRING:
    return "string";
  case TYPE_TAG_BOOLEAN:
    return "bool";
  case TYPE_TAG_ANY:
    return "any";
  default:
    return "";
  }
}
