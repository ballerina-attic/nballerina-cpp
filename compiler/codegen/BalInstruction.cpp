#include "interfaces/Instruction.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"

namespace nballerina {

Instruction::Instruction(Operand *lOp, BasicBlock *currB)
    : lhsOp(lOp), currentBB(currB) {}

Operand *Instruction::getLhsOperand() { return lhsOp; }
Function *Instruction::getFunction() { return currentBB->getFunction(); }
Package *Instruction::getPkgAddress() {
  return currentBB->getFunction()->getPkgAddress();
}

} // namespace nballerina
