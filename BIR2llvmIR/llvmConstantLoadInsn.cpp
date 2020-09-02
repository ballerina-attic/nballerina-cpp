#include "BIR.h"

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

  LLVMValueRef constRef  = LLVMConstInt(LLVMInt64Type(), value, 0);

  LLVMValueRef loaded  = LLVMBuildStore(builder, constRef, lhsRef);
}
