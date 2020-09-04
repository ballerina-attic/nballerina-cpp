#include "BIR.h"

GoToInsn::GoToInsn() {
}

GoToInsn::GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 BasicBlockT *nextBB){
}
GoToInsn::~GoToInsn() {

}

void GoToInsn::translate(LLVMModuleRef &modRef) {
  if (thenBB->getLLVMBasicBlock())
    LLVMBuildBr(LLVMBuilderRef, thenBB->getLLVMBasicBlock());  
  else {
    fprintf(stderr, "%s:%d LLVM Basic Block not found for GOTO instruction.\n",
            __FILE__, __LINE__);
  }
}
