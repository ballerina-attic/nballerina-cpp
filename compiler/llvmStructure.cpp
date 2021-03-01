#include "BIR.h"

void StructureInsn::translate(LLVMModuleRef &modRef) {

  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();

  string lhsName = getLhsOperand()->name();

  // Find VarDecl corresponding to lhs to determine structure and member type
  VarDecl *lhsVar = funcObj->getLocalVarDeclFromName(lhsName);
  if (!lhsVar) {
    lhsVar = pkgObj->getGlobalVarDeclFromName(lhsName);
  }
  assert(lhsVar);

  // Determine structure type
  int structType = lhsVar->getTypeDecl()->getTypeTag();

  // Only handle Map type
  if (structType != TYPE_TAG_MAP) {
    std::cerr << "Non MAP type structs are currently not supported"
              << std::endl;
    assert(false);
  }
  mapInsnTranslate(lhsVar, modRef);
}

void StructureInsn::mapInsnTranslate(VarDecl *lhsVar, LLVMModuleRef &modRef) {

  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  string lhsName = getLhsOperand()->name();
  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

  assert(lhsVar->getTypeDecl()->getTypeTag() == TYPE_TAG_MAP);
  MapTypeDecl *mapTypeDelare =
      static_cast<MapTypeDecl *>(lhsVar->getTypeDecl());

  // Get member type
  int memberTypeTag = mapTypeDelare->getTypeMemberTag();
  // Only handle Int type
  if (memberTypeTag != TYPE_TAG_INT) {
    std::cerr << "Non INT type maps are currently not supported" << std::endl;
    assert(false);
  }

  // Codegen for Map of Int type
  LLVMValueRef newMapIntFunc = pkgObj->getFunctionRefBasedOnName("map_new_int");
  if (!newMapIntFunc)
    newMapIntFunc = getNewMapIntDeclaration(modRef, pkgObj);
  assert(newMapIntFunc);
  LLVMValueRef newMapIntRef =
      LLVMBuildCall(builder, newMapIntFunc, nullptr, 0, "");
  LLVMBuildStore(builder, newMapIntRef, lhsOpRef);
}

// Declaration for new map<int> function
LLVMValueRef StructureInsn::getNewMapIntDeclaration(LLVMModuleRef &modRef,
                                                    BIRPackage *pkg) {
  LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt8Type(), 0);
  LLVMTypeRef funcType = LLVMFunctionType(memPtrType, nullptr, 0, 0);
  LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "map_new_int", funcType);
  pkg->addArrayFunctionRef("map_new_int", addedFuncRef);
  return addedFuncRef;
}
