#include "BalMoveInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalVarDecl.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

MoveInsn::MoveInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp) {}

void MoveInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {
  LLVMValueRef lhsRef;
  Operand *lhsOp = getLhsOperand();

  if (!lhsOp->getVarDecl())
    return;

  VarKind varKind = lhsOp->getVarDecl()->getVarKind();
  assert(getFunction());
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();

  if (!(lhsOp && lhsOp->getVarDecl()))
    return;

  if (varKind == GLOBAL_VAR_KIND) {
    lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsOp->getName());
    LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);
    LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
  } else if (varKind == LOCAL_VAR_KIND || varKind == TEMP_VAR_KIND ||
             varKind == RETURN_VAR_KIND) {
    lhsRef = getFunction()->getLocalVarRefUsingId(lhsOp->getName());
    LLVMValueRef rhsVarOpRef = getFunction()->getLocalToTempVar(rhsOp);
    LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
  } else
    llvm_unreachable("Unknown varkind");
}

} // namespace nballerina
