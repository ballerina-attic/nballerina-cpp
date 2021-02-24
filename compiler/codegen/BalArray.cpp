#include "BalArrayInsns.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "llvm-c/Core.h"
#include <string>

using namespace std;

namespace nballerina {

// New Array Instruction
ArrayInsn::ArrayInsn(Operand *lOp, Operand *sOp, TypeDecl *tDecl)
    : NonTerminatorInsn(lOp), sizeOp(sOp), typeDecl(tDecl) {}

LLVMValueRef ArrayInsn::getNewArrayDeclaration(LLVMModuleRef &modRef,
                                               Package *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[1];
  paramTypes[0] = LLVMInt32Type();
  LLVMTypeRef funcType = LLVMFunctionType(LLVMInt32Type(), paramTypes, 1, 0);
  LLVMValueRef addedFuncRef =
      LLVMAddFunction(modRef, "new_int_array", funcType);
  pkg->addArrayFunctionRef("new_int_array", addedFuncRef);
  return addedFuncRef;
}

Operand *ArrayInsn::getSizeOp() { return sizeOp; }
TypeDecl *ArrayInsn::getTypeDecl() { return typeDecl; }

void ArrayInsn::translate(LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  Package *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->getName();
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

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(Operand *lOp, bool opFA, bool fR, Operand *KOp,
                             Operand *ROp)
    : NonTerminatorInsn(lOp), optionalFieldAccess(opFA), fillingRead(fR),
      keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef,
                                                    Package *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[2];
  LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
  paramTypes[0] = int32PtrType;
  paramTypes[1] = LLVMInt32Type();
  LLVMTypeRef funcType = LLVMFunctionType(int32PtrType, paramTypes, 2, 0);
  LLVMValueRef addedFuncRef =
      LLVMAddFunction(modRef, "int_array_load", funcType);
  pkg->addArrayFunctionRef("int_array_load", addedFuncRef);
  return addedFuncRef;
}

bool ArrayLoadInsn::getOptionalFieldAccess() { return optionalFieldAccess; }
bool ArrayLoadInsn::getFillingRead() { return fillingRead; }
Operand *ArrayLoadInsn::getKeyOp() { return keyOp; }
Operand *ArrayLoadInsn::getRhsOp() { return rhsOp; }

void ArrayLoadInsn::translate(LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  Package *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->getName();
  string rhsName = rhsOp->getName();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  LLVMValueRef ArrayLoadFunc =
      pkgObj->getFunctionRefBasedOnName("int_array_load");
  if (!ArrayLoadFunc)
    ArrayLoadFunc = getArrayLoadDeclaration(modRef, pkgObj);
  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

  LLVMValueRef rhsOpRef = funcObj->getLocalVarRefUsingId(rhsName);
  LLVMValueRef keyRef = funcObj->getLocalToTempVar(keyOp);
  assert(ArrayLoadFunc && rhsOpRef && keyRef);
  LLVMValueRef *sizeOpValueRef = new LLVMValueRef[2];
  sizeOpValueRef[0] = rhsOpRef;
  sizeOpValueRef[1] = keyRef;
  LLVMValueRef valueInArrayPointer =
      LLVMBuildCall(builder, ArrayLoadFunc, sizeOpValueRef, 2, "");
  LLVMValueRef ArrayTempVal = LLVMBuildLoad(builder, valueInArrayPointer, "");
  LLVMBuildStore(builder, ArrayTempVal, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(Operand *lOp, Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(lOp), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef,
                                                      Package *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[3];
  LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
  paramTypes[0] = int32PtrType;
  paramTypes[1] = LLVMInt32Type();
  paramTypes[2] = int32PtrType;
  LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
  LLVMValueRef addedFuncRef =
      LLVMAddFunction(modRef, "int_array_store", funcType);
  pkg->addArrayFunctionRef("int_array_store", addedFuncRef);
  return addedFuncRef;
}

Operand *ArrayStoreInsn::getKeyOp() { return keyOp; }
Operand *ArrayStoreInsn::getRhsOp() { return rhsOp; }

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  Package *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->getName();
  string rhsName = rhsOp->getName();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  LLVMValueRef ArrayLoadFunc =
      pkgObj->getFunctionRefBasedOnName("int_array_store");
  if (!ArrayLoadFunc)
    ArrayLoadFunc = getArrayStoreDeclaration(modRef, pkgObj);

  LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsOpRef)
    lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

  LLVMValueRef rhsOpRef = funcObj->getLocalVarRefUsingId(rhsName);
  LLVMValueRef keyRef = funcObj->getLocalToTempVar(keyOp);
  assert(ArrayLoadFunc && lhsOpRef && rhsOpRef && keyRef);
  LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
  argOpValueRef[0] = lhsOpRef;
  argOpValueRef[1] = keyRef;
  argOpValueRef[2] = rhsOpRef;

  LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
