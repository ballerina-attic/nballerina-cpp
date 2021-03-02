#include "interfaces/BalAbstractInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"

namespace nballerina {

AbstractInsn::AbstractInsn(Operand *lOp, BasicBlock *currB)
    : lhsOp(lOp), currentBB(currB) {}

Operand *AbstractInsn::getLhsOperand() { return lhsOp; }
Function *AbstractInsn::getFunction() { return currentBB->getFunction(); }
Package *AbstractInsn::getPkgAddress() {
  return currentBB->getFunction()->getPkgAddress();
}

} // namespace nballerina
