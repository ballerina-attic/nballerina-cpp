#include "BIR.h"

TerminatorInsn::TerminatorInsn(Location *pos, InstructionKind kind,
                               Operand *lOp, BIRBasicBlock *then)
    : AbstractInsn(pos, kind, lOp), thenBB(then) {}

void TerminatorInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}