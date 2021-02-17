#include "BIR.h"

AbstractInsn::AbstractInsn(Location *pos, InstructionKind kindP, Operand *lOp)
    : BIRNode(pos), kind(kindP), lhsOp(lOp) {}
