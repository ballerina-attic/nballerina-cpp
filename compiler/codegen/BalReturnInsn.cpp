#include "BalReturnInsn.h"
#include "BalFunction.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;
using namespace llvm;

namespace nballerina {

ReturnInsn::ReturnInsn(BasicBlock *currentBB)
    : TerminatorInsn(nullptr, currentBB, nullptr, false) {}

void ReturnInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

  Function *funcObj = getFunction();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  Variable *globRetVar =
      getPkgAddress()->getGlobalVarDeclFromName("_bal_result");

  if (funcObj->getName() == "main" && globRetVar) {
    LLVMValueRef lhsRef =
        getPkgAddress()->getGlobalVarRefUsingId("_bal_result");
    LLVMValueRef retValRef = LLVMBuildLoad(builder, lhsRef, "retrun_temp");
    LLVMBuildRet(builder, retValRef);
  } else {
    if (funcObj->getReturnVar()->getTypeDecl()->getTypeTag() != TYPE_TAG_NIL) {
      LLVMValueRef retValueRef = LLVMBuildLoad(
          builder, funcObj->getLocalVarRefUsingId("%0"), "retrun_temp");
      LLVMBuildRet(builder, retValueRef);
    } else if (builder) {
      LLVMBuildRetVoid(builder);
    }
  }
}

} // namespace nballerina