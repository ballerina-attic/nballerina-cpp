#include "BalNonTerminatorInsn.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

NonTerminatorInsn::NonTerminatorInsn(Location *posi, InstructionKind kindP,
                                     Operand *lOp)
    : AbstractInsn(posi, kindP, lOp) {}

void NonTerminatorInsn::translate(__attribute__((unused))
                                  LLVMModuleRef &modRef) {}

} // namespace nballerina
