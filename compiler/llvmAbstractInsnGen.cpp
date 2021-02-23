#include "BIR.h"

AbstractInsn::AbstractInsn(Location *pos, InstructionKind kindP, Operand *lOp)
    : kind(kindP), lhsOp(lOp) {
  setLocation(pos);
}
