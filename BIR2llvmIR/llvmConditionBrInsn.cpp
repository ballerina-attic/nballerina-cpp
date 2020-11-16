#include "BIR.h"

ConditionBrInsn::ConditionBrInsn() {}
ConditionBrInsn::ConditionBrInsn(Location *pos, InstructionKind kind,
                                 Operand *lOp, BIRBasicBlock *nextBB)
    : TerminatorInsn(pos, kind, lOp, nextBB) {}
ConditionBrInsn::~ConditionBrInsn() {}

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
  if (getFunction() && lhsName != "") {
    brCondition = getFunction()->getValueRefBasedOnName(lhsName);
  }

  if (getIfThenBB() && getElseBB()) {
    ifLLVMBB = getIfThenBB()->getLLVMBBRef();
    elseLLVMBB = getElseBB()->getLLVMBBRef();
  }
  if (builder && brCondition && ifLLVMBB && elseLLVMBB) {
    LLVMBuildCondBr(builder, brCondition, ifLLVMBB, elseLLVMBB);
  }
}
