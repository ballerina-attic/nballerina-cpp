#include "BIR.h"

Move::Move(){
}

Move::Move(Operand lOp,Operand rOp):
	NonTerminatorInsn(lOp),rhsOp(rOp){
}

Move::Move(BIRFunction *bFun):
		NonTerminatorInsn(bFun) {
}

Operand Move::getrhsOp(){
  return rhsOp;
}

Move::~Move(){
}

void Move::translate(){
  LLVMBuilderRef builder = getFunction()->getllvmBuilder();

  LLVMValueRef lhsRef = getFunction()->getlocalVarRefusingId(
			getlhsOperand().getvarDecl()->getvarName());

  LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);

  LLVMBuildStore(builder, rhsVarOpRef, lhsRef);  
}
