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
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
  if(getLhsOperand())
  {
    LLVMValueRef lhsRef = getFunction()->getLocalVarRefUsingId(
                        getLhsOperand()->getVarDecl()->getVarName());
    LLVMValueRef retValRef = LLVMBuildLoad(builder, lhsRef, "retrun_temp");
    LLVMBuildRet(builder, retValRef);
  }
  else
  {
    LLVMBuildRetVoid(builder);
  }
}
