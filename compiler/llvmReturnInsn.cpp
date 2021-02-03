#include "BIR.h"

ReturnInsn::ReturnInsn() {}

ReturnInsn::ReturnInsn(Location *pos, InstructionKind kind, Operand *lOp,
                       BIRBasicBlock *nextBB)
    : TerminatorInsn(pos, kind, lOp, nextBB) {}

ReturnInsn::~ReturnInsn() {}

void ReturnInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  BIRFunction *funcObj = getFunction();
  VarDecl *returnVarDecl = NULL;
  if (funcObj) {
    builder = funcObj->getLLVMBuilder();
    vector<VarDecl *> globalVarList = getPkgAddress()->getGlobalVars();
    for (unsigned int i = 0; i < globalVarList.size(); i++) {
      VarDecl *varDeclLoc = globalVarList[i];
      string varDeclName = varDeclLoc->getVarName();
      if (varDeclName == "_bal_result") {
        returnVarDecl = varDeclLoc;
        break;
      }
    }
  }
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
    assert(funcObj && funcObj->getReturnVar() &&
           funcObj->getReturnVar()->getTypeDecl());
    if (funcObj->getReturnVar()->getTypeDecl()->getTypeTag() !=
        TYPE_TAG_NIL) {
      LLVMValueRef retValueRef = LLVMBuildLoad(
          builder, funcObj->getLocalVarRefUsingId("%0"), "retrun_temp");
      LLVMBuildRet(builder, retValueRef);
    } else if (builder) {
        LLVMBuildRetVoid(builder);
    }
  }
}
