#include "BIR.h"

NonTerminatorInsn::NonTerminatorInsn(Location *posi, InstructionKind kindP,
                                     Operand *lOp)
    : AbstractInsn(posi, kindP, lOp) {}

void NonTerminatorInsn::translate(__attribute__((unused))
                                  LLVMModuleRef &modRef) {}