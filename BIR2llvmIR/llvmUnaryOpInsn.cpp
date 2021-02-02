#include "BIR.h"

UnaryOpInsn::UnaryOpInsn() {}

UnaryOpInsn::UnaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp,
                         Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp) {}

UnaryOpInsn::~UnaryOpInsn() {}

void UnaryOpInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  LLVMBuilderRef builder;
  string lhstmpName;
  LLVMValueRef lhsRef;
  LLVMValueRef rhsOpref;
  LLVMValueRef ifReturn;
  Operand *lhsOp = getLhsOperand();
  string lhsName = lhsOp->name();

  if (funcObj && lhsOp && lhsOp->getVarDecl()) {
    builder = funcObj->getLLVMBuilder();
    lhstmpName = lhsName + "_temp";
    lhsRef = funcObj->getLocalVarRefUsingId(lhsName);
    if (!lhsRef)
      lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);
  }

  if (funcObj && rhsOp)
    rhsOpref = funcObj->getLocalToTempVar(rhsOp);

  if (rhsOpref && builder) {
    switch (getInstKind()) {
    case INSTRUCTION_KIND_UNARY_NOT: {
      ifReturn = LLVMBuildNot(builder, rhsOpref, lhstmpName.c_str());
      break;
    }
    case INSTRUCTION_KIND_UNARY_NEG: {
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
