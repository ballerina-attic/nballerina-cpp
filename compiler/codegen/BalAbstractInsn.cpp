#include "BalAbstractInsn.h"

namespace nballerina {
AbstractInsn::AbstractInsn(Location *pos, InstructionKind kindP, Operand *lOp)
    : kind(kindP), lhsOp(lOp) {
  setLocation(pos);
}
InstructionKind AbstractInsn::getInstKind() { return kind; }
Operand *AbstractInsn::getLhsOperand() { return lhsOp; }
BIRFunction *AbstractInsn::getFunction() { return bFunc; }
BIRBasicBlock *AbstractInsn::getCurrentBB() { return currentBB; }

void AbstractInsn::setFunction(BIRFunction *func) { bFunc = func; }
void AbstractInsn::setInstKind(InstructionKind newKind) { kind = newKind; }
void AbstractInsn::setLhsOperand(Operand *lOp) { lhsOp = lOp; }
void AbstractInsn::setCurrentBB(BIRBasicBlock *currB) { currentBB = currB; }

} // namespace nballerina
