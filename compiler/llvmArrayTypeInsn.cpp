#include "BIR.h"

ArrayInsn::ArrayInsn() {}

ArrayInsn::ArrayInsn(Location *pos, InstructionKind kind, Operand *lOp,
                     Operand *sOp, TypeDecl *tDecl)
    : NonTerminatorInsn(pos, kind, lOp), sizeOp(sOp), typeDecl(tDecl) {}

LLVMValueRef ArrayInsn::getNewArrayDeclaration(LLVMModuleRef &modRef,
                                               BIRPackage *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[1];
  paramTypes[0] = LLVMInt32Type();
  LLVMTypeRef funcType = LLVMFunctionType(LLVMInt32Type(), paramTypes, 1, 0);
  LLVMValueRef addedFuncRef =
      LLVMAddFunction(modRef, "new_int_array", funcType);
  pkg->addArrayFunctionRef("new_int_array", addedFuncRef);
  return addedFuncRef;
}

void ArrayInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->name();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  LLVMValueRef *sizeOpValueRef = new LLVMValueRef[1];
  LLVMValueRef localTempCarRef = funcObj->getLocalToTempVar(sizeOp);
  sizeOpValueRef[0] = localTempCarRef;
  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);
  LLVMValueRef newArrayFunc =
      pkgObj->getFunctionRefBasedOnName("new_int_array");
  if (!newArrayFunc)
    newArrayFunc = getNewArrayDeclaration(modRef, pkgObj);
  assert(sizeOpValueRef && newArrayFunc);
  LLVMValueRef newArrayRef =
      LLVMBuildCall(builder, newArrayFunc, sizeOpValueRef, 1, "");

  LLVMBuildStore(builder, newArrayRef, lhsOpRef);
}

ArrayInsn::~ArrayInsn() {}
