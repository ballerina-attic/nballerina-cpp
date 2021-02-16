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

  LLVMValueRef newStructFunc = pkgObj->getFunctionRefBasedOnName("map_new_int");
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
  LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "map_new_int", funcType);
  pkg->addArrayFunctionRef("map_new_int", addedFuncRef);
  return addedFuncRef;
}

MapStoreInsn::MapStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef MapStoreInsn::getMapStoreDeclaration(LLVMModuleRef &modRef,
                                                  BIRPackage *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[3];
  LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
  LLVMTypeRef charArrayPtrType = LLVMPointerType(LLVMInt8Type(), 0);
  paramTypes[0] = int32PtrType;
  paramTypes[1] = charArrayPtrType;
  paramTypes[2] = int32PtrType;
  LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
  LLVMValueRef addedFuncRef =
      LLVMAddFunction(modRef, "map_store_int", funcType);
  pkg->addArrayFunctionRef("map_store_int", addedFuncRef);
  return addedFuncRef;
}

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->name();
  string rhsName = rhsOp->name();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  LLVMValueRef mapStoreFunc =
      pkgObj->getFunctionRefBasedOnName("map_store_int");
  if (!mapStoreFunc)
    mapStoreFunc = getMapStoreDeclaration(modRef, pkgObj);

  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

  LLVMValueRef rhsOpRef = funcObj->getLocalVarRefUsingId(rhsName);
  LLVMValueRef keyRef = funcObj->getLocalToTempVar(keyOp);
  assert(mapStoreFunc && lhsOpRef && rhsOpRef && keyRef);
  LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
  argOpValueRef[0] = lhsOpRef;
  argOpValueRef[1] = keyRef;
  argOpValueRef[2] = rhsOpRef;

  LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}
