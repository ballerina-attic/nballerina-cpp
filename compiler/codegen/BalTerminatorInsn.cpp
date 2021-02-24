#include "BalTerminatorInsn.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

TerminatorInsn::TerminatorInsn(Location *pos, InstructionKind kind,
                               Operand *lOp, BasicBlock *then)
    : AbstractInsn(pos, kind, lOp), thenBB(then) {}

BasicBlock *TerminatorInsn::getNextBB() { return thenBB; }
bool TerminatorInsn::getPatchStatus() { return patchRequire; }

void TerminatorInsn::setNextBB(BasicBlock *block) { thenBB = block; }
void TerminatorInsn::setPatchStatus(bool patchrequire) {
  patchRequire = patchrequire;
}

void TerminatorInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}

} // namespace nballerina
