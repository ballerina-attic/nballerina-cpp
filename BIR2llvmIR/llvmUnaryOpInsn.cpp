#include "BIR.h"

UnaryOpInsn::UnaryOpInsn() {
}

UnaryOpInsn::UnaryOpInsn (Location *pos, InstructionKind kind,
				Operand *lOp, Operand *rOp):
		NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp) {
}

UnaryOpInsn::~UnaryOpInsn() {
}

void UnaryOpInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  string lhstmpName;
  LLVMValueRef lhsRef;
  LLVMValueRef rhsOpref; 
  LLVMValueRef ifReturn;
  if (getFunction() && getLhsOperand() && getLhsOperand()->getVarDecl()) {
    builder = getFunction()->getLLVMBuilder();
    lhstmpName = getLhsOperand()->getVarDecl()->getVarName() + "_temp" ;
    lhsRef = getFunction()->getLocalVarRefUsingId(
                                getLhsOperand()->getVarDecl()->getVarName());
  }

  if (getFunction() && rhsOp)
    rhsOpref = getFunction()->getLocalToTempVar(rhsOp);

  if (rhsOpref && builder) {
    switch (getInstKind()) {
      case INSTRUCTION_KIND_UNARY_NOT:
      {
  	ifReturn = LLVMBuildNot(builder, rhsOpref, lhstmpName.c_str());
        break;
      }
      case INSTRUCTION_KIND_UNARY_NEG:
      {
        ifReturn = LLVMBuildNeg(builder, rhsOpref, lhstmpName.c_str());
        break;
      }    
      default:
        break;
    }
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }
  }
}
