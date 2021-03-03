#include "BalConditionBrInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {

ConditionBrInsn::ConditionBrInsn(Operand *lOp, BasicBlock *currentBB,
                                 BasicBlock *_ifThenBB, BasicBlock *_elseBB)
    : TerminatorInsn(lOp, currentBB, nullptr, true), ifThenBB(_ifThenBB),
      elseBB(_elseBB) {
  kind = INSTRUCTION_KIND_CONDITIONAL_BRANCH;
}

BasicBlock *ConditionBrInsn::getIfThenBB() { return ifThenBB; }
BasicBlock *ConditionBrInsn::getElseBB() { return elseBB; }
void ConditionBrInsn::setIfThenBB(BasicBlock *bb) { ifThenBB = bb; }
void ConditionBrInsn::setElseBB(BasicBlock *bb) { elseBB = bb; }

void ConditionBrInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
  string lhsName = getLHS()->getName();

  LLVMValueRef brCondition =
      getFunction()->getLLVMValueForBranchComparison(lhsName);
  if (!brCondition) {
    brCondition = LLVMBuildIsNotNull(
        builder, getFunction()->getTempLocalVariable(getLHS()),
        lhsName.c_str());
  }

  LLVMBuildCondBr(builder, brCondition, ifThenBB->getLLVMBBRef(),
                  elseBB->getLLVMBBRef());
}

} // namespace nballerina
