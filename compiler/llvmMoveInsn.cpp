#include "BIR.h"

MoveInsn::MoveInsn() {}

MoveInsn::MoveInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp) {}

MoveInsn::~MoveInsn() {}

void MoveInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  LLVMValueRef lhsRef;
  Operand *lhsOp = getLhsOperand();
  VarKind varKind;
  if (lhsOp->getVarDecl())
    varKind = lhsOp->getVarDecl()->getVarKind();

  if (getFunction())
    builder = getFunction()->getLLVMBuilder();

  if (lhsOp && lhsOp->getVarDecl()) {
    if (varKind == GLOBAL_VAR_KIND) {
      lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsOp->name());
      LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);
      LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
    }

    else if (varKind == LOCAL_VAR_KIND || varKind == TEMP_VAR_KIND ||
             varKind == RETURN_VAR_KIND) {
      lhsRef = getFunction()->getLocalVarRefUsingId(lhsOp->name());
      LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);
      LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
    }
  }
}
