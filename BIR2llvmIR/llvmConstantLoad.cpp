#include "BIR.h"

ConstantLoad::ConstantLoad (unsigned long long val, Operand lOp):
				value(val), NonTerminatorInsn(lOp) {

}

ConstantLoad::ConstantLoad (BIRFunction *bFun):NonTerminatorInsn(bFun) {

}

ConstantLoad::~ConstantLoad() {
}

void ConstantLoad::translate() {
  LLVMBuilderRef builder = getFunction()->getllvmBuilder();
  
  LLVMValueRef lhsRef    = getFunction()->getlocalVarRefusingId(
                            getlhsOperand().getvarDecl()->getvarName());

  LLVMValueRef constRef  = LLVMConstInt(LLVMInt64Type(), value, 0);

  LLVMValueRef loaded  = LLVMBuildStore(builder, constRef, lhsRef);
}
