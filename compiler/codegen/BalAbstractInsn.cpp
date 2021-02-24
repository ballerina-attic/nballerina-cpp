#include "BalAbstractInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"

namespace nballerina {

AbstractInsn::AbstractInsn(Operand *lOp) : lhsOp(lOp) {}
Operand *AbstractInsn::getLhsOperand() { return lhsOp; }
Function *AbstractInsn::getFunction() { return currentBB->getFunction(); }
BasicBlock *AbstractInsn::getCurrentBB() { return currentBB; }
Package *AbstractInsn::getPkgAddress() {
  return currentBB->getFunction()->getPkgAddress();
}

void AbstractInsn::setCurrentBB(BasicBlock *currB) { currentBB = currB; }

} // namespace nballerina
