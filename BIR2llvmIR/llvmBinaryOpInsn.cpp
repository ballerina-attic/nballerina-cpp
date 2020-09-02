#include "BIR.h"

BinaryOpInsn::BinaryOpInsn (Location *pos, InstructionKind kind,
				Operand *lOp, Operand *rOp1, Operand *rOp2):
		NonTerminatorInsn(pos, kind, lOp), rhsOp1(rOp1), rhsOp2(rOp2) {
}

BinaryOpInsn::~BinaryOpInsn() {
}

void BinaryOpInsn::translate(LLVMModuleRef &modRef){
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
  string lhstmpName = getLhsOperand()->getVarDecl()->getVarName() + "_temp" ;
  LLVMValueRef lhsRef = getFunction()->getLocalVarRefUsingId(
				getLhsOperand()->getVarDecl()->getVarName());

  LLVMValueRef rhsOp1ref = getFunction()->getLocalToTempVar(rhsOp1);
  LLVMValueRef rhsOp2ref = getFunction()->getLocalToTempVar(rhsOp2);

  switch (getInstKind()) {
    case BINARY_ADD:
    {
      LLVMValueRef addRet = LLVMBuildAdd(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      LLVMValueRef loaded = LLVMBuildStore(builder, addRet, lhsRef);
    }
    case BINARY_SUB:
    {
      LLVMValueRef subRet = LLVMBuildSub(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      LLVMValueRef loaded = LLVMBuildStore(builder, subRet, lhsRef);
    }
    case BINARY_MUL:
    {
      LLVMValueRef mulRet = LLVMBuildMul(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      LLVMValueRef loaded = LLVMBuildStore(builder, mulRet, lhsRef);
    }
    case BINARY_DIV:
    {
      LLVMValueRef divRet = LLVMBuildSDiv(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      LLVMValueRef loaded = LLVMBuildStore(builder, divRet, lhsRef);
    }
    case BINARY_EQUAL:
    {
      LLVMValueRef equalRet = LLVMBuildICmp(builder, LLVMIntEQ, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      LLVMValueRef loaded = LLVMBuildStore(builder, equalRet, lhsRef);
    }
  }
}
