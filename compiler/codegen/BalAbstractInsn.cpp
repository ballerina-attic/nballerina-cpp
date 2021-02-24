#include "BalAbstractInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"

namespace nballerina {

AbstractInsn::AbstractInsn(Location *pos, InstructionKind kindP, Operand *lOp)
    : kind(kindP), lhsOp(lOp) {
  setLocation(pos);
}
InstructionKind AbstractInsn::getInstKind() { return kind; }
Operand *AbstractInsn::getLhsOperand() { return lhsOp; }
Function *AbstractInsn::getFunction() { return currentBB->getFunction(); }
BasicBlock *AbstractInsn::getCurrentBB() { return currentBB; }
Package *AbstractInsn::getPkgAddress() {
  return currentBB->getFunction()->getPkgAddress();
};

void AbstractInsn::setInstKind(InstructionKind newKind) { kind = newKind; }
void AbstractInsn::setLhsOperand(Operand *lOp) { lhsOp = lOp; }
void AbstractInsn::setCurrentBB(BasicBlock *currB) { currentBB = currB; }

} // namespace nballerina
