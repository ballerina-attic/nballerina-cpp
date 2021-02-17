#include "BIR.h"

NonTerminatorInsn::NonTerminatorInsn(Location *posi, InstructionKind kindP,
                                     Operand *lOp)
    : AbstractInsn(posi, kindP, lOp) {}
