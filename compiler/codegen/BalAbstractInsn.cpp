#include "BalAbstractInsn.h"

namespace nballerina {

AbstractInsn::AbstractInsn(Location *pos, InstructionKind kindP, Operand *lOp)
    : kind(kindP), lhsOp(lOp) {
  setLocation(pos);
}
InstructionKind AbstractInsn::getInstKind() { return kind; }
Operand *AbstractInsn::getLhsOperand() { return lhsOp; }
Function *AbstractInsn::getFunction() { return bFunc; }
BasicBlock *AbstractInsn::getCurrentBB() { return currentBB; }

void AbstractInsn::setFunction(Function *func) { bFunc = func; }
void AbstractInsn::setInstKind(InstructionKind newKind) { kind = newKind; }
void AbstractInsn::setLhsOperand(Operand *lOp) { lhsOp = lOp; }
void AbstractInsn::setCurrentBB(BasicBlock *currB) { currentBB = currB; }

} // namespace nballerina
