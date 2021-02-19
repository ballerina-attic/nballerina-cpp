#include "BIR.h"

GoToInsn::GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   BIRBasicBlock *nextBB)
    : TerminatorInsn(pos, kind, lOp, nextBB) {}

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
