#include "BIR.h"

ArrayLoadInsn::ArrayLoadInsn() {}

ArrayLoadInsn::ArrayLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                             bool opFA, bool fR, Operand *KOp, Operand *ROp)
    : NonTerminatorInsn(pos, kind, lOp), optionalFieldAccess(opFA),
      fillingRead(fR), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef,
                                                    BIRPackage *pkg) {
  LLVMTypeRef *paramTypes = new LLVMTypeRef[2];
  LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
  LLVMTypeRef int8PtrType = LLVMPointerType(LLVMInt8Type(), 0);
  paramTypes[0] = int8PtrType;
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
  LLVMValueRef rhsOpLoad = LLVMBuildLoad(builder, rhsOpRef, "");
  LLVMValueRef keyRef = funcObj->getLocalToTempVar(keyOp);
  assert(ArrayLoadFunc && rhsOpRef && keyRef);
  LLVMValueRef *sizeOpValueRef = new LLVMValueRef[2];
  sizeOpValueRef[0] = rhsOpLoad;
  sizeOpValueRef[1] = keyRef;
  LLVMValueRef valueInArrayPointer =
      LLVMBuildCall(builder, ArrayLoadFunc, sizeOpValueRef, 2, "");
  LLVMValueRef ArrayTempVal = LLVMBuildLoad(builder, valueInArrayPointer, "");
  LLVMBuildStore(builder, ArrayTempVal, lhsOpRef);
}

ArrayLoadInsn::~ArrayLoadInsn() {}
