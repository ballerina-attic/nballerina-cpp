#include "BIR.h"

TerminatorInsn::TerminatorInsn(Location *pos, InstructionKind kind,
                               Operand *lOp, BIRBasicBlock *then)
    : AbstractInsn(pos, kind, lOp), thenBB(then) {}
