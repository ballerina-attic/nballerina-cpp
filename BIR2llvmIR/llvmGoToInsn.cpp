#include "BIR.h"

GoToInsn::GoToInsn() {
}

GoToInsn::GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 BasicBlockT *nextBB){
}
GoToInsn::~GoToInsn() {

}

void GoToInsn::translate(LLVMModuleRef &modRef) {
  LLVMBasicBlockRef bummyBBRef;
  LLVMBuildBr(LLVMBuilderRef, bummyBBRef);  
}
