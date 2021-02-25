#include "BalTerminatorInsn.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

TerminatorInsn::TerminatorInsn(Operand *lOp, BasicBlock *currentBB,
                               BasicBlock *then, bool _patchRequire)
    : AbstractInsn(lOp, currentBB), thenBB(then), patchRequire(_patchRequire) {}

BasicBlock *TerminatorInsn::getNextBB() { return thenBB; }
bool TerminatorInsn::getPatchStatus() { return patchRequire; }
InstructionKind TerminatorInsn::getInstKind() { return kind; }
void TerminatorInsn::setPatched() { patchRequire = false; }
void TerminatorInsn::setNextBB(BasicBlock *bb) { thenBB = bb; }

void TerminatorInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {}

} // namespace nballerina
