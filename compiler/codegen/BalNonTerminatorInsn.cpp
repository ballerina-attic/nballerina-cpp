#include "BalNonTerminatorInsn.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

NonTerminatorInsn::NonTerminatorInsn(Operand *lOp) : AbstractInsn(lOp) {}

void NonTerminatorInsn::translate(__attribute__((unused))
                                  LLVMModuleRef &modRef) {}

} // namespace nballerina
