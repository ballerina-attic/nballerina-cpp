#include "BIR.h"

GoToInsn::GoToInsn() {}

GoToInsn::GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 BIRBasicBlock *nextBB): TerminatorInsn(pos,kind,lOp,nextBB) {}


GoToInsn::~GoToInsn() {}

void GoToInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  if (getFunction())
    builder = getFunction()->getLLVMBuilder();

  if (builder && getNextBB() && getNextBB()->getLLVMBBRef())
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());  
  else {
    fprintf(stderr, "%s:%d LLVM Basic Block not found for GOTO instruction.\n",
            __FILE__, __LINE__);
  }
}
