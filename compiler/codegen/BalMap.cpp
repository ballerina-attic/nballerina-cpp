#include "BalEnums.h"
#include "BalFunction.h"
#include "BalMapInsns.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalTypeDecl.h"
#include "BalVarDecl.h"
#include "llvm-c/Core.h"
#include <iostream>

using namespace std;

namespace nballerina {

// new Map Instruction and Codegen logic are in the llvmStructure.cpp

MapStoreInsn::MapStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), keyOp(KOp), rhsOp(rOp) {}

void MapStoreInsn::setKeyOp(Operand *kOp) { keyOp = kOp; }
void MapStoreInsn::setRhsOp(Operand *rOp) { rhsOp = rOp; }
Operand *MapStoreInsn::getKeyOp() { return keyOp; }
Operand *MapStoreInsn::getRhsOp() { return rhsOp; }

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->getName();
  string rhsName = rhsOp->getName();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();

  // Find VarDecl corresponding to lhs to determine member type
  VarDecl *lhsVar = funcObj->getNameVarDecl(lhsName);
  assert(lhsVar);
  assert(lhsVar->getVarKind() == LOCAL_VAR_KIND);
  assert(lhsVar->getTypeDecl()->getTypeTag() == TYPE_TAG_MAP);
  MapTypeDecl *mapTypeDelare =
      static_cast<MapTypeDecl *>(lhsVar->getTypeDecl());
  int memberTypeTag = mapTypeDelare->getTypeMemberTag();

  // Only handle Int type
  if (memberTypeTag != TYPE_TAG_INT) {
    std::cerr << "Non INT type maps are currently not supported" << std::endl;
    assert(false);
  }

  // Codegen for Map of Int type store
  LLVMValueRef mapStoreFunc =
      pkgObj->getFunctionRefBasedOnName("map_store_int");
  if (!mapStoreFunc)
    mapStoreFunc = getMapIntStoreDeclaration(modRef, pkgObj);

  LLVMValueRef lhsOpTempRef = funcObj->getLocalToTempVar(getLhsOperand());

  LLVMValueRef rhsOpRef = funcObj->getLocalVarRefUsingId(rhsName);
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
  LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt64Type(), 0);
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