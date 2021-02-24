#include "BalGoToInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

GoToInsn::GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   BasicBlock *nextBB)
    : TerminatorInsn(pos, kind, lOp, nextBB) {}

LLVMValueRef GoToInsn::getLLVMInsn() { return llvmInsn; }
void GoToInsn::setLLVMInsn(LLVMValueRef insn) { llvmInsn = insn; }

void GoToInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {
  assert(getFunction());
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();

  if (builder && getNextBB() && getNextBB()->getLLVMBBRef())
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
  else {
    fprintf(stderr, "%s:%d LLVM Basic Block not found for GOTO instruction.\n",
            __FILE__, __LINE__);
  }
}

} // namespace nballerina