#include "BIR.h"

ReturnInsn::ReturnInsn() {

}

ReturnInsn::ReturnInsn(Location *pos, InstructionKind kind, 
			Operand *lOp, BasicBlockT *nextBB):
	TerminatorInsn(pos, kind, lOp, nextBB) {

}

ReturnInsn::~ReturnInsn() {
}

void ReturnInsn::translat(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  VarDecl* returnVarDecl = NULL;
  if (getFunction())
    builder = getFunction()->getLLVMBuilder();

  vector<VarDecl *> globalVarList = getpkgAddress()->getGlobalVars();
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
    if (!lhsRef)
      lhsRef = getpkgAddress()->getGlobalVarRefUsingId(
			returnVarDecl->getVarName());
    
    LLVMValueRef retValRef = LLVMBuildLoad(builder, lhsRef, "retrun_temp");
    if (retValRef)
      LLVMBuildRet(builder, retValRef);
  }
  else {
    if (builder)
      LLVMBuildRetVoid(builder);
  }
}
