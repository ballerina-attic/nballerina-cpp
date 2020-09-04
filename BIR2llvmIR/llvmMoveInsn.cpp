#include "BIR.h"

MoveInsn::MoveInsn(){
}

MoveInsn::MoveInsn(Location *pos, InstructionKind kind,Operand *lOp, Operand *rOp):
	NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp) {
}

MoveInsn::~MoveInsn(){
}

void MoveInsn::translate(LLVMModuleRef &modRef){
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();

  LLVMValueRef lhsRef = getFunction()->getLocalVarRefUsingId(
			getLhsOperand()->getVarDecl()->getVarName());

  LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);

  LLVMBuildStore(builder, rhsVarOpRef, lhsRef);  
}
