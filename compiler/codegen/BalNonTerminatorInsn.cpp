#include "BalNonTerminatorInsn.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

NonTerminatorInsn::NonTerminatorInsn(Operand *lOp, BasicBlock *currentBB)
    : Instruction(lOp, currentBB) {}

void NonTerminatorInsn::translate(__attribute__((unused))
                                  LLVMModuleRef &modRef) {}

} // namespace nballerina
