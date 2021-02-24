#include "BalTerminatorInsn.h"

namespace nballerina {

TerminatorInsn::TerminatorInsn(Location *pos, InstructionKind kind,
                               Operand *lOp, BIRBasicBlock *then)
    : AbstractInsn(pos, kind, lOp), thenBB(then) {}

BIRBasicBlock *TerminatorInsn::getNextBB() { return thenBB; }
bool TerminatorInsn::getPatchStatus() { return patchRequire; }

void TerminatorInsn::setNextBB(BIRBasicBlock *block) { thenBB = block; }
void TerminatorInsn::setPatchStatus(bool patchrequire) {
  patchRequire = patchrequire;
}

void TerminatorInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}

} // namespace nballerina
