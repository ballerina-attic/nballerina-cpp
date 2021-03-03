#include "BalFunction.h"
#include "BalMapInsns.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"
#include <iostream>

using namespace std;

namespace nballerina {

// new Map Instruction and Codegen logic are in the llvmStructure.cpp

MapStoreInsn::MapStoreInsn(Operand *lOp, BasicBlock *currentBB, Operand *KOp,
                           Operand *rOp)
    : NonTerminatorInsn(lOp, currentBB), keyOp(KOp), rhsOp(rOp) {}

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  Package *pkgObj = getPackage();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();

  // Find Variable corresponding to lhs to determine member type
  Variable *lhsVar = funcObj->getLocalOrGlobalVariable(getLHS());
  assert(lhsVar->getTypeDecl()->getTypeTag() == TYPE_TAG_MAP);

  MapTypeDecl *mapTypeDelare =
      static_cast<MapTypeDecl *>(lhsVar->getTypeDecl());
  TypeTag memberTypeTag = mapTypeDelare->getMemberTypeTag();

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

  LLVMValueRef lhsOpTempRef = funcObj->getTempLocalVariable(getLHS());
  LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
  LLVMValueRef keyRef = funcObj->getTempLocalVariable(keyOp);
  assert(mapStoreFunc && lhsOpTempRef && rhsOpRef && keyRef);
  
  LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
  argOpValueRef[0] = lhsOpTempRef;
  argOpValueRef[1] = keyRef;
  argOpValueRef[2] = rhsOpRef;

  LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

// Declaration for map<int> type store function
LLVMValueRef MapStoreInsn::getMapIntStoreDeclaration(LLVMModuleRef &modRef,
                                                     Package *pkg) {
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

} // namespace nballerina
