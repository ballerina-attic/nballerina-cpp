#include "BalNonTerminatorInsn.h"

namespace nballerina {

NonTerminatorInsn::NonTerminatorInsn(Location *posi, InstructionKind kindP,
                                     Operand *lOp)
    : AbstractInsn(posi, kindP, lOp) {}

void NonTerminatorInsn::translate(__attribute__((unused))
                                  LLVMModuleRef &modRef) {}

} // namespace nballerina
