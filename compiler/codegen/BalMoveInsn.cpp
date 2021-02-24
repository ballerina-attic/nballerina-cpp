// #include "BalMoveInsn.h"
// #include "llvm-c/Core.h"
#include "../BIR.h" // temp till package and func and available

namespace nballerina {

MoveInsn::MoveInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp) {}

void MoveInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {
  LLVMValueRef lhsRef;
  Operand *lhsOp = getLhsOperand();
  VarKind varKind;
  if (lhsOp->getVarDecl())
    varKind = lhsOp->getVarDecl()->getVarKind();

  assert(getFunction());
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();

  if (!(lhsOp && lhsOp->getVarDecl()))
    return;

  if (varKind == GLOBAL_VAR_KIND) {
    lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsOp->name());
    LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);
    LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
  } else if (varKind == LOCAL_VAR_KIND || varKind == TEMP_VAR_KIND ||
             varKind == RETURN_VAR_KIND) {
    lhsRef = getFunction()->getLocalVarRefUsingId(lhsOp->name());
    LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);
    LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
  } else
    llvm_unreachable("Unknown varkind");
}

} // namespace nballerina
