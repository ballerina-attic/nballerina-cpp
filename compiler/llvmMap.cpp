#include "BIR.h"

// new Map Instruction and Codegen logic are in the llvmStructure.cpp

MapStoreInsn::MapStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), keyOp(KOp), rhsOp(rOp) {}

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->name();
  string rhsName = rhsOp->name();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();

  // Find VarDecl corresponding to lhs to determine member type
  VarDecl *lhsVar = funcObj->getLocalVarDeclFromName(lhsName);
  if (!lhsVar) {
    lhsVar = pkgObj->getGlobalVarDeclFromName(lhsName);
  }
  assert(lhsVar);

  assert(lhsVar->getTypeDecl()->getTypeTag() == TYPE_TAG_MAP);
  MapTypeDecl *mapTypeDelare =
      static_cast<MapTypeDecl *>(lhsVar->getTypeDecl());
  int memberTypeTag = mapTypeDelare->getTypeMemberTag();

  // Only handle Int type
  if (memberTypeTag != TYPE_TAG_INT) {
    std::cerr << "Non INT type maps are currently not supported" << std::endl;
    llvm_unreachable("Unknown Type");
  }

  // Codegen for Map of Int type store
  LLVMValueRef mapStoreFunc =
      pkgObj->getFunctionRefBasedOnName("map_store_int");
  if (!mapStoreFunc)
    mapStoreFunc = getMapIntStoreDeclaration(modRef, pkgObj);

  LLVMValueRef lhsOpTempRef = funcObj->getLocalToTempVar(getLhsOperand());

  LLVMValueRef rhsOpRef = funcObj->getLocalVarRefUsingId(rhsName);
  if (!rhsOpRef)
    rhsOpRef = pkgObj->getGlobalVarRefUsingId(rhsName);

  LLVMValueRef keyRef = funcObj->getLocalToTempVar(keyOp);
  assert(mapStoreFunc && lhsOpTempRef && rhsOpRef && keyRef);
  LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
  argOpValueRef[0] = lhsOpTempRef;
  argOpValueRef[1] = keyRef;
  argOpValueRef[2] = rhsOpRef;

  LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

// Declaration for map<int> type store function
LLVMValueRef MapStoreInsn::getMapIntStoreDeclaration(LLVMModuleRef &modRef,
                                                     BIRPackage *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[3];
  LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
  LLVMTypeRef charArrayPtrType = LLVMPointerType(LLVMInt8Type(), 0);
  LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt8Type(), 0);
  paramTypes[0] = memPtrType;
  paramTypes[1] = charArrayPtrType;
  paramTypes[2] = int32PtrType;
  LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
  LLVMValueRef addedFuncRef =
      LLVMAddFunction(modRef, "map_store_int", funcType);
  pkg->addArrayFunctionRef("map_store_int", addedFuncRef);
  return addedFuncRef;
}
