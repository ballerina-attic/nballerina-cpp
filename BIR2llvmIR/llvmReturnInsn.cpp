#include "BIR.h"

ReturnInsn::ReturnInsn() {}

ReturnInsn::ReturnInsn(Location *pos, InstructionKind kind, 
			Operand *lOp, BIRBasicBlock *nextBB):
	TerminatorInsn(pos, kind, lOp, nextBB) {}

ReturnInsn::~ReturnInsn() {}

void ReturnInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  VarDecl* returnVarDecl = NULL;
  if (getFunction())
    builder = getFunction()->getLLVMBuilder();
  if (getFunction()->getName() == "main") {
    vector<VarDecl *> globalVarList = getPkgAddress()->getGlobalVars();
    for (unsigned int i = 0; i < globalVarList.size(); i++)
    {
      VarDecl * varDeclLoc = globalVarList[i];
      string varDeclName = varDeclLoc->getVarName();
      if (varDeclName == "_bal_result") {
        returnVarDecl = varDeclLoc;
        break;
      }
    }
    if(builder && getFunction() && returnVarDecl) {
      LLVMValueRef lhsRef = getFunction()->getLocalVarRefUsingId(
                          returnVarDecl->getVarName());
      if (!lhsRef) {
        lhsRef = getPkgAddress()->getGlobalVarRefUsingId(
  			returnVarDecl->getVarName());
      
        LLVMValueRef retValRef = LLVMBuildLoad(builder, lhsRef, "retrun_temp");
        if (retValRef)
          LLVMBuildRet(builder, retValRef);
      }
    }
  }
  else if (getFunction() && getFunction()->getReturnVar() && 
	  getFunction()->getReturnVar()->getTypeDecl() && 
	  getFunction()->getReturnVar()->getTypeDecl()->getTypeTag() != 
		TYPE_TAG_NIL && builder) {
      LLVMValueRef retValueRef = LLVMBuildLoad(builder, getFunction()->getLocalVarRefUsingId("%0"), "retrun_temp");
      LLVMBuildRet(builder, retValueRef);
  }
  else {
    if (builder)
      LLVMBuildRetVoid(builder);
  }
}
