#include "BIR.h"

void StructureInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();

  string lhsName = getLhsOperand()->name();
  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

  // to optimize; early return and in variant checking
  int memberTypeTag = 0;
  auto vars = funcObj->getLocalVars();
  for (auto var : vars) {
    if (var->getVarName() == lhsName) {
      assert(var->getVarKind() == LOCAL_VAR_KIND);
      assert(var->getTypeDecl()->getTypeTag() == TYPE_TAG_MAP);
      MapTypeDecl *mapTypeDelare =
          static_cast<MapTypeDecl *>(var->getTypeDecl());
      memberTypeTag = mapTypeDelare->getTypeMemberTag();
      break;
    }
  }
  assert(memberTypeTag);

  if (memberTypeTag != TYPE_TAG_INT)
    std::cerr << "Non init type maps are currently not supported" << std::endl;

  LLVMValueRef newStructFunc =
      pkgObj->getFunctionRefBasedOnName("new_map_init");
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
  LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "new_map_init", funcType);
  pkg->addArrayFunctionRef("new_map_init", addedFuncRef);
  return addedFuncRef;
}