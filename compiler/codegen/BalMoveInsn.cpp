#include "BalMoveInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

MoveInsn::MoveInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp) : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp) {}

void MoveInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

    LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
    LLVMValueRef lhsRef = getFunction()->getLLVMLocalOrGlobalVar(getLHS());
    LLVMValueRef rhsVarOpRef = getFunction()->getTempLocalVariable(rhsOp);
    LLVMBuildStore(builder, rhsVarOpRef, lhsRef);
}

} // namespace nballerina
