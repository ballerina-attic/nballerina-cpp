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
  if (getFunction())
    builder = getFunction()->getLLVMBuilder();
  if(builder && getFunction() && getLhsOperand() && 
		getLhsOperand()->getVarDecl()) {
    LLVMValueRef lhsRef = getFunction()->getLocalVarRefUsingId(
                        getLhsOperand()->getVarDecl()->getVarName());
    LLVMValueRef retValRef = LLVMBuildLoad(builder, lhsRef, "retrun_temp");
    if (retValRef)
      LLVMBuildRet(builder, retValRef);
  }
  else {
    if (builder)
      LLVMBuildRetVoid(builder);
  }
}
