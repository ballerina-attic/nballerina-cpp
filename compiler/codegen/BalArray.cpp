#include "BalArrayInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "llvm-c/Core.h"
#include <string>

using namespace std;

namespace nballerina {
ArrayLoadInsn::ArrayLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                             bool opFA, bool fR, Operand *KOp, Operand *ROp)
    : NonTerminatorInsn(pos, kind, lOp), optionalFieldAccess(opFA),
      fillingRead(fR), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef,
                                                    BIRPackage *pkg) {
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

void ArrayLoadInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->name();
  string rhsName = rhsOp->name();
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

ArrayStoreInsn::ArrayStoreInsn(Location *pos, InstructionKind kind,
                               Operand *lOp, Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef,
                                                      BIRPackage *pkg) {
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

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  BIRPackage *pkgObj = getPkgAddress();
  string lhsName = getLhsOperand()->name();
  string rhsName = rhsOp->name();
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

} // namespace nballerina
