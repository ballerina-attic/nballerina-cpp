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
  Operand *lhsOp = getLhsOperand();

  if (getFunction() && lhsOp && lhsOp->getVarDecl()) {
    builder = getFunction()->getLLVMBuilder();
    lhsName = lhsOp->name();
  }
  if (getFunction() && lhsName != "") {
    brCondition = getFunction()->getValueRefBasedOnName(lhsName);
  }

  if (!brCondition) {
    VarDecl *lhsVarDecl = getFunction()->getLocalVarDeclFromName(lhsName);
    if (lhsVarDecl->getTypeDecl()->getTypeTag() == TYPE_TAG_BOOLEAN) {
      brCondition = LLVMBuildIsNotNull(
          builder, getFunction()->getLocalToTempVar(lhsOp), lhsName.c_str());
    }
  }

  if (getIfThenBB() && getElseBB()) {
    ifLLVMBB = getIfThenBB()->getLLVMBBRef();
    elseLLVMBB = getElseBB()->getLLVMBBRef();
  }
  if (builder && brCondition && ifLLVMBB && elseLLVMBB) {
    LLVMBuildCondBr(builder, brCondition, ifLLVMBB, elseLLVMBB);
  }
}
