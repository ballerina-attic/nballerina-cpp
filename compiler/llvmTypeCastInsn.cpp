#include "BIR.h"
#ifndef unix
#define __attribute__(unused)
#endif

TypeCastInsn::TypeCastInsn() {}

TypeCastInsn::TypeCastInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp, TypeDecl *tDecl,
                           bool checkTypes)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp), typeDecl(tDecl), checkTypes(checkTypes) {}

TypeCastInsn::~TypeCastInsn() {}

void TypeCastInsn::translate(LLVMModuleRef &modRef) {
    BIRFunction *funcObj = getFunction();
    string lhsOpName = getLhsOperand()->name();
    string rhsOpName = rhsOp->name();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef rhsOpRef;
    LLVMValueRef lhsOpRef;
    LLVMTypeRef lhsTypeRef;
    if (funcObj) {
        rhsOpRef = funcObj->getLocalVarRefUsingId(rhsOpName);
        if (!rhsOpRef)
            rhsOpRef = getPkgAddress()->getGlobalVarRefUsingId(rhsOpName);
        lhsOpRef = funcObj->getLocalVarRefUsingId(lhsOpName);
        if (!lhsOpRef)
            lhsOpRef = getPkgAddress()->getGlobalVarRefUsingId(lhsOpName);
        lhsTypeRef = LLVMTypeOf(lhsOpRef);
        VarDecl *orignamVarDecl = funcObj->getLocalVarDeclFromName(rhsOpName);
        StringTableBuilder *strTable = getPkgAddress()->getStrTableBuilder();
        if (orignamVarDecl && orignamVarDecl->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {

            // GEP of last type of smart pointer(original type of any variable(smart
            // pointer))
            LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, rhsOpRef, 1, "lastTypeIdx");
            LLVMValueRef lastTypeLoad = LLVMBuildLoad(builder, lastTypeIdx, "");
            // sign extent of GEP to i64.
            LLVMValueRef sExt = LLVMBuildSExt(builder, lastTypeLoad, LLVMInt64Type(), "");
            // Data object of smart pointer.
            LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 2, "data");
            LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");

            LLVMValueRef strTblPtr = getPkgAddress()->getGlobalVarRefUsingId("__string_table_ptr");
            LLVMValueRef strTblLoad = LLVMBuildLoad(builder, strTblPtr, "");
            LLVMValueRef *sizeOpValueRef = new LLVMValueRef[1];
            sizeOpValueRef[0] = sExt;
            LLVMValueRef gepOfStr = LLVMBuildInBoundsGEP(builder, strTblLoad, sizeOpValueRef, 1, "");

            // get the mangled name of the lhs type and store it to string
            // builder table.
            VarDecl *origVarDecl = funcObj->getLocalVarDeclFromName(lhsOpName);
            assert(origVarDecl->getTypeDecl()->getTypeTag());
            TypeTagEnum lhsTypeTag = TypeTagEnum(origVarDecl->getTypeDecl()->getTypeTag());
            char const *lhsTypeName = typeStringMangleName(lhsOpRef, lhsTypeTag);
            if (!strTable->contains(lhsTypeName))
                strTable->add(lhsTypeName);
            int tempRandNum = rand() % 1000 + 1;
            LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum, 0);
            sizeOpValueRef[0] = constValue;
            LLVMValueRef lhsGep = LLVMBuildInBoundsGEP(builder, strTblLoad, sizeOpValueRef, 1, "");
            // call is_same_type rust function to check LHS and RHS type are same or
            // not.
            LLVMValueRef addedIsSameTypeFunc = isSameType(modRef, lhsGep, gepOfStr);
            LLVMValueRef *paramRefs = new LLVMValueRef[2];
            paramRefs[0] = lhsGep;
            paramRefs[1] = gepOfStr;
            assert(addedIsSameTypeFunc);
            LLVMValueRef sameTypeResult = LLVMBuildCall(builder, addedIsSameTypeFunc, paramRefs, 2, "call");
            // creating branch condition using is_same_type() function result.
            LLVMValueRef brCondition __attribute__((unused)) = LLVMBuildIsNotNull(builder, sameTypeResult, "");
            getPkgAddress()->addStringOffsetRelocationEntry(lhsTypeName, lhsGep);

            LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, lhsOpName.c_str());
            LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
            LLVMBuildStore(builder, castLoad, lhsOpRef);
        } else if (getLhsOperand() &&
                   funcObj->getLocalVarDeclFromName(lhsOpName)->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {
            LLVMValueRef structAllocaRef = funcObj->getLocalVarRefUsingId(getLhsOperand()->name());

            // struct first element original type
            LLVMValueRef origTypeIdx = LLVMBuildStructGEP(builder, structAllocaRef, 0, "origTypeIdx");
            VarDecl *origVarDecl = funcObj->getLocalVarDeclFromName(lhsOpName);
            assert(origVarDecl->getTypeDecl()->getTypeTag());
            TypeTagEnum origTypeTag = TypeTagEnum(origVarDecl->getTypeDecl()->getTypeTag());
            char const *origTypeName = typeStringMangleName(lhsOpRef, origTypeTag);
            if (!strTable->contains(origTypeName))
                strTable->add(origTypeName);
            int tempRandNum1 = rand() % 1000 + 1;
            LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum1, 0);
            LLVMValueRef origStoreRef = LLVMBuildStore(builder, constValue, origTypeIdx);
            getPkgAddress()->addStringOffsetRelocationEntry(origTypeName, origStoreRef);
            // struct second element last type
            LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, structAllocaRef, 1, "lastTypeIdx");
            VarDecl *lastTypeVarDecl = funcObj->getLocalVarDeclFromName(rhsOpName);
            assert(lastTypeVarDecl->getTypeDecl()->getTypeTag());
            TypeTagEnum lastTypeTag = TypeTagEnum(lastTypeVarDecl->getTypeDecl()->getTypeTag());
            char const *lastTypeName = typeStringMangleName(rhsOpRef, lastTypeTag);
            if (!strTable->contains(lastTypeName))
                strTable->add(lastTypeName);
            int tempRandNum2 = rand() % 1000 + 1;
            LLVMValueRef constValue1 = LLVMConstInt(LLVMInt32Type(), tempRandNum2, 0);
            LLVMValueRef lastStoreRef = LLVMBuildStore(builder, constValue1, lastTypeIdx);
            getPkgAddress()->addStringOffsetRelocationEntry(lastTypeName, lastStoreRef);

            // struct third element void pointer data.
            LLVMValueRef elePtr2 = LLVMBuildStructGEP(builder, structAllocaRef, 2, "data");
            LLVMValueRef bitCastRes1 = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
            LLVMBuildStore(builder, bitCastRes1, elePtr2);
        } else {
            LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
        }
    }
}

LLVMValueRef TypeCastInsn::getIsSameTypeDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg, LLVMValueRef lhsRef,
                                                    LLVMValueRef rhsRef) {
    LLVMTypeRef *paramTypes = new LLVMTypeRef[2];
    paramTypes[0] = LLVMTypeOf(lhsRef);
    paramTypes[1] = LLVMTypeOf(rhsRef);
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt8Type(), paramTypes, 2, 0);
    LLVMValueRef addedFuncRef = LLVMAddFunction(modRef, "is_same_type", funcType);
    pkg->addArrayFunctionRef("is_same_type", addedFuncRef);
    return addedFuncRef;
}

LLVMValueRef TypeCastInsn::isSameType(LLVMModuleRef &modRef, LLVMValueRef lhsRef, LLVMValueRef rhsRef) {
    BIRPackage *pkgObj = getPkgAddress();
    LLVMValueRef addedFuncRef = pkgObj->getFunctionRefBasedOnName("is_same_type");
    if (!addedFuncRef)
        addedFuncRef = getIsSameTypeDeclaration(modRef, pkgObj, lhsRef, rhsRef);
    return addedFuncRef;
}

char const *TypeCastInsn::typeStringMangleName(LLVMValueRef typeVal, TypeTagEnum typeTag) {
    char const *finalString;
    switch (typeTag) {
    case TYPE_TAG_INT: {
        finalString = "__I";
        break;
    }
    case TYPE_TAG_FLOAT: {
        finalString = "__F";
        break;
    }
    case TYPE_TAG_CHAR_STRING: {
        finalString = "__C";
        break;
    }
    case TYPE_TAG_STRING: {
        finalString = "__S";
        break;
    }
    case TYPE_TAG_BOOLEAN: {
        finalString = "__B";
        break;
    }
    case TYPE_TAG_ARRAY: {
        LLVMTypeRef valType = LLVMTypeOf(typeVal);
        // TODO Need to add Size of the array.
        if (unwrap(valType)->getPointerElementType()->isIntegerTy())
            finalString = "__AI";
        else
            finalString = "__A";
        break;
    }
    case TYPE_TAG_ANY: {
        finalString = "__X";
        break;
    }
    default:
        return "";
    }
    return finalString;
}
