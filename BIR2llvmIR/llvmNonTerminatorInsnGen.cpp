#include "BIR.h"

NonTerminatorInsn::NonTerminatorInsn() {}

NonTerminatorInsn::NonTerminatorInsn(Location *posi, InstructionKind kindP,
                                     Operand *lOp)
    : AbstractInsn(posi, kindP, lOp) {}

NonTerminatorInsn::~NonTerminatorInsn() {}

void NonTerminatorInsn::translate(LLVMModuleRef &modRef) {}
