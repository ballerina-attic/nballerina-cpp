#include "BIR.h"

GoToInsn::GoToInsn() {
}

GoToInsn::GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 BasicBlockT *nextBB): TerminatorInsn(pos,kind,lOp,nextBB){
}


GoToInsn::~GoToInsn() {

}

void GoToInsn::translat(LLVMModuleRef &modRef) 
{
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
  if (getNextBB()->getLLVMBBRef())
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());  
  else {
    fprintf(stderr, "%s:%d LLVM Basic Block not found for GOTO instruction.\n",
            __FILE__, __LINE__);
  }
}

