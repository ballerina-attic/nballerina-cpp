#include "BalReturnInsn.h"
#include "BalFunction.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVarDecl.h"
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
  VarDecl *returnVarDecl = nullptr;

  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  VarDecl *globRetVar =
      getPkgAddress()->getGlobalVarDeclFromName("_bal_result");
  if (globRetVar)
    returnVarDecl = globRetVar;

  if (funcObj->getName() == "main" && returnVarDecl) {
    if (builder && funcObj) {
      LLVMValueRef lhsRef =
          funcObj->getLocalVarRefUsingId(returnVarDecl->getVarName());
      if (!lhsRef) {
        lhsRef = getPkgAddress()->getGlobalVarRefUsingId(
            returnVarDecl->getVarName());

        LLVMValueRef retValRef = LLVMBuildLoad(builder, lhsRef, "retrun_temp");
        if (retValRef)
          LLVMBuildRet(builder, retValRef);
      }
    }
  } else {
    assert(funcObj->getReturnVar()->getTypeDecl());
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