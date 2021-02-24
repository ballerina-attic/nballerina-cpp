#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalStructureInsn.h"
#include "BalTypeDecl.h"
#include "llvm-c/Core.h"
#include <iostream>

using namespace std;
using namespace llvm;

namespace nballerina {

void StructureInsn::translate(LLVMModuleRef &modRef) {

  Function *funcObj = getFunction();
  string lhsName = getLhsOperand()->getName();

  // Find VarDecl corresponding to lhs to determine structure and member type
  VarDecl *lhsVar = funcObj->getNameVarDecl(lhsName);
  assert(lhsVar);

  // Determine structure type
  assert(lhsVar->getVarKind() == LOCAL_VAR_KIND);
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

  Function *funcObj = getFunction();
  Package *pkgObj = getPkgAddress();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  string lhsName = getLhsOperand()->getName();
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
                                                    Package *pkg) {
  LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt64Type(), 0);
  LLVMTypeRef funcType = LLVMFunctionType(memPtrType, nullptr, 0, 0);
  LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "map_new_int", funcType);
  pkg->addArrayFunctionRef("map_new_int", addedFuncRef);
  return addedFuncRef;
}

} // namespace nballerina