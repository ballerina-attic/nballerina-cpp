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
  Operand *lhsOp = getLhsOperand();
  string lhsName = lhsOp->getName();
  string lhstmpName = lhsName + "_temp";

  LLVMValueRef lhsRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsRef)
    lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);

  assert(rhsOp);
  LLVMValueRef rhsOpref = funcObj->getLocalToTempVar(rhsOp);
  if (!rhsOpref)
    return;

  LLVMValueRef ifReturn = LLVMBuildNot(builder, rhsOpref, lhstmpName.c_str());
  LLVMBuildStore(builder, ifReturn, lhsRef);
  return;
}

} // namespace nballerina