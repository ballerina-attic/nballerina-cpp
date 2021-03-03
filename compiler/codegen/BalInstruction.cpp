#include "interfaces/Instruction.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"

namespace nballerina {

Instruction::Instruction(Operand *lOp, BasicBlock *currB)
    : lhsOp(lOp), parentBB(currB) {}

Operand *Instruction::getLHS() { return lhsOp; }
Function *Instruction::getFunction() { return parentBB->getFunction(); }
Package *Instruction::getPackage() {
  return parentBB->getFunction()->getPkgAddress();
}

} // namespace nballerina
