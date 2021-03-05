#include "BIR.h"

ArrayStoreInsn::ArrayStoreInsn() {}

ArrayStoreInsn::ArrayStoreInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(pos, kind, lOp), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg) {
    LLVMTypeRef *paramTypes = new LLVMTypeRef[3];
    LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
    LLVMTypeRef int8PtrType = LLVMPointerType(LLVMInt8Type(), 0);
    paramTypes[0] = int8PtrType;
    paramTypes[1] = LLVMInt32Type();
    paramTypes[2] = int32PtrType;
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "int_array_store", funcType);
    pkg->addArrayFunctionRef("int_array_store", addedFuncRef);
    return addedFuncRef;
}

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
    BIRFunction *funcObj = getFunction();
    BIRPackage *pkgObj = getPkgAddress();
    string lhsName = getLhsOperand()->name();
    string rhsName = rhsOp->name();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = pkgObj->getFunctionRefBasedOnName("int_array_store");
    if (!ArrayLoadFunc)
        ArrayLoadFunc = getArrayStoreDeclaration(modRef, pkgObj);

    LLVMValueRef lhsOpRef = funcObj->getLocalVarRefUsingId(lhsName);
    if (!lhsOpRef)
        lhsOpRef = pkgObj->getGlobalVarRefUsingId(lhsName);

    LLVMValueRef rhsOpRef = funcObj->getLocalVarRefUsingId(rhsName);
    LLVMValueRef keyRef = funcObj->getLocalToTempVar(keyOp);
    assert(ArrayLoadFunc && lhsOpRef && rhsOpRef && keyRef);
    LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
    LLVMValueRef lhsOpLoad = LLVMBuildLoad(builder, lhsOpRef, "");
    argOpValueRef[0] = lhsOpLoad;
    argOpValueRef[1] = keyRef;
    argOpValueRef[2] = rhsOpRef;

    LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

ArrayStoreInsn::~ArrayStoreInsn() {}
