#include "BalBasicBlock.h"
#include "BalConditionBrInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalTypeDecl.h"
#include "BalVarDecl.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {

ConditionBrInsn::ConditionBrInsn(Location *pos, InstructionKind kind,
                                 Operand *lOp, BIRBasicBlock *nextBB)
    : TerminatorInsn(pos, kind, lOp, nextBB) {}

void ConditionBrInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

  Operand *lhsOp = getLhsOperand();
  assert(lhsOp && lhsOp->getVarDecl());
  assert(getFunction());
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();

  string lhsName = lhsOp->name();
  assert(lhsName != "");
  LLVMValueRef brCondition = getFunction()->getValueRefBasedOnName(lhsName);

  if (!brCondition) {
    VarDecl *lhsVarDecl = getFunction()->getNameVarDecl(lhsName);
    if (lhsVarDecl->getTypeDecl()->getTypeTag() == TYPE_TAG_BOOLEAN) {
      brCondition = LLVMBuildIsNotNull(
          builder, getFunction()->getLocalToTempVar(lhsOp), lhsName.c_str());
    }
  }

  if (!getIfThenBB() || !getElseBB())
    return;

  LLVMBasicBlockRef ifLLVMBB = getIfThenBB()->getLLVMBBRef();
  LLVMBasicBlockRef elseLLVMBB = getElseBB()->getLLVMBBRef();

  if (builder && brCondition && ifLLVMBB && elseLLVMBB) {
    LLVMBuildCondBr(builder, brCondition, ifLLVMBB, elseLLVMBB);
  }
}

} // namespace nballerina