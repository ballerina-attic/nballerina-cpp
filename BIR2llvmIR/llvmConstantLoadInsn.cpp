#include "BIR.h"

ConstantLoadInsn::ConstantLoadInsn () {
  value = 0;
}

ConstantLoadInsn::ConstantLoadInsn (Location *pos, InstructionKind kind,
			Operand *lOp, unsigned long long val):
		NonTerminatorInsn(pos, kind, lOp), value(val) {

}

ConstantLoadInsn::~ConstantLoadInsn() {
}

void ConstantLoadInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
  
  LLVMValueRef lhsRef    = getFunction()->getLocalVarRefUsingId(
                            getLhsOperand()->getVarDecl()->getVarName());

  LLVMValueRef constRef  = LLVMConstInt(LLVMInt32Type(), value, 0);

  LLVMBuildStore(builder, constRef, lhsRef);
}
