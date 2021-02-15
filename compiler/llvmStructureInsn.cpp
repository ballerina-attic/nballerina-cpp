#include "BIR.h"

void StructureInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->name();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();

  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

  LLVMValueRef newStructFunc = pkgObj->getFunctionRefBasedOnName("ruvi");
  if (!newStructFunc)
    newStructFunc = getNewStructureDeclaration(modRef, pkgObj);
  assert(newStructFunc);
  LLVMValueRef newStructRef =
      LLVMBuildCall(builder, newStructFunc, nullptr, 0, "");

  LLVMBuildStore(builder, newStructRef, lhsOpRef);
}

LLVMValueRef StructureInsn::getNewStructureDeclaration(LLVMModuleRef &modRef,
                                                       BIRPackage *pkg) {
  LLVMTypeRef funcType = LLVMFunctionType(LLVMInt32Type(), nullptr, 0, 0);
  LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "ruvi", funcType);
  pkg->addArrayFunctionRef("ruvi", addedFuncRef);
  return addedFuncRef;
}