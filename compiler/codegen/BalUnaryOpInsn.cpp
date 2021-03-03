#include "BalUnaryOpInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include <llvm-c/Core.h>
#include <string>

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {

UnaryOpInsn::UnaryOpInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp) {}

void UnaryOpInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

  Function *funcObj = getFunction();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  Operand *lhsOp = getLHS();
  string lhsName = lhsOp->getName();
  string lhstmpName = lhsName + "_temp";
  LLVMValueRef lhsRef = funcObj->getLLVMLocalOrGlobalVar(lhsOp);
  LLVMValueRef rhsOpref = funcObj->getTempLocalVariable(rhsOp);
  LLVMValueRef ifReturn = LLVMBuildNot(builder, rhsOpref, lhstmpName.c_str());
  LLVMBuildStore(builder, ifReturn, lhsRef);
  return;
}

} // namespace nballerina
