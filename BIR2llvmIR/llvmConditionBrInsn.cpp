#include "BIR.h"

ConditionBrInsn::ConditionBrInsn() {

}
ConditionBrInsn::ConditionBrInsn (Location *pos, InstructionKind kind,
                        Operand *lOp, BasicBlockT *nextBB):
                        TerminatorInsn(pos, kind, lOp, nextBB) {

}
ConditionBrInsn::~ConditionBrInsn() {

}

void ConditionBrInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  string lhsName;
  LLVMValueRef brCondition;
  LLVMBasicBlockRef ifLLVMBB;
  LLVMBasicBlockRef elseLLVMBB;
  if (getFunction() && getLhsOperand() && getLhsOperand()->getVarDecl()) {
    builder = getFunction()->getLLVMBuilder();
    lhsName = getLhsOperand()->getVarDecl()->getVarName();
  }
  if (getcurrentBB() && lhsName != "") {
    brCondition = getcurrentBB()->getValueRefBasedOnName(lhsName);
  }
  
  if (getifThenBB() && getelseBB()) {
    ifLLVMBB = getifThenBB()->getLLVMBBRef();
    elseLLVMBB = getelseBB()->getLLVMBBRef();
  }
  if (builder && brCondition && ifLLVMBB && elseLLVMBB) {
    LLVMValueRef condBrRes = LLVMBuildCondBr(builder, brCondition,
					ifLLVMBB, elseLLVMBB);
  }
}
