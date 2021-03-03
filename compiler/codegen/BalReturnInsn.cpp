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
  LLVMValueRef globRetRef = getPackage()->getGlobalLLVMVar("_bal_result");

  if (funcObj->getName() == "main" && globRetRef) {
    LLVMValueRef retValRef = LLVMBuildLoad(builder, globRetRef, "retrun_temp");
    LLVMBuildRet(builder, retValRef);
    return;
  }

  if (funcObj->getReturnVar()->getTypeDecl()->getTypeTag() == TYPE_TAG_NIL) {
    LLVMBuildRetVoid(builder);
    return;
  }

  LLVMValueRef retValueRef =
      LLVMBuildLoad(builder, funcObj->getLLVMLocalVar("%0"), "retrun_temp");
  LLVMBuildRet(builder, retValueRef);
}

} // namespace nballerina
