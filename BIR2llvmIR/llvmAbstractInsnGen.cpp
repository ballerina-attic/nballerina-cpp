#include "BIR.h"

AbstractInsn::AbstractInsn() {}

AbstractInsn::AbstractInsn(Location pos, InstructionKind kindP):BIRNode(pos), kind(kindP){}

AbstractInsn::AbstractInsn(Operand lOp):lhsOp(lOp) {
}

AbstractInsn::~AbstractInsn() {}

void AbstractInsn::translate() { }

InstructionKind AbstractInsn::getInstKind() {
  return kind;
}

Operand AbstractInsn::getlhsOperand(){
  return lhsOp;
}
