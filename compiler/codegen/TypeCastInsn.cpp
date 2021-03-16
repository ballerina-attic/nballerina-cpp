/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "TypeCastInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

namespace nballerina {

TypeCastInsn::TypeCastInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp, [[maybe_unused]] Type *tDecl,
                           [[maybe_unused]] bool checkTypes)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp) {}

void TypeCastInsn::translate([[maybe_unused]] LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    Package *pkgObj = getPackage();
    const std::string lhsOpName = getLHS()->getName();
    const std::string rhsOpName = rhsOp->getName();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();

    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMTypeRef lhsTypeRef = LLVMTypeOf(lhsOpRef);
    Variable *orignamVarDecl = funcObj->getLocalOrGlobalVariable(rhsOp);
    StringTableBuilder *strTable = pkgObj->getStrTableBuilder();
    const char *lastTypeChar = "lastTypeIdx";
    const char *origTypeChar = "origTypeIdx";
    if (orignamVarDecl && orignamVarDecl->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {
        // GEP of last type of smart pointer(original type of any variable(smart
        // pointer))
        LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, rhsOpRef, 1, lastTypeChar);
        LLVMValueRef lastTypeLoad = LLVMBuildLoad(builder, lastTypeIdx, "");
        // sign extent of GEP to i64.
        LLVMValueRef sExt = LLVMBuildSExt(builder, lastTypeLoad, LLVMInt64Type(), "");
        // Data object of smart pointer.
        LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 2, "data");
        LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");
        LLVMValueRef strTblPtr = pkgObj->getStringBuilderTableGlobalPointer();
        LLVMValueRef strTblLoad = LLVMBuildLoad(builder, strTblPtr, "");
        LLVMValueRef *sizeOpValueRef = new LLVMValueRef[1];
        sizeOpValueRef[0] = sExt;
        LLVMValueRef gepOfStr = LLVMBuildInBoundsGEP(builder, strTblLoad, sizeOpValueRef, 1, "");

        // get the mangled name of the lhs type and store it to string
        // builder table.
        Variable *origVarDecl = funcObj->getLocalOrGlobalVariable(getLHS());
        assert(origVarDecl->getTypeDecl()->getTypeTag());
        TypeTag lhsTypeTag = TypeTag(origVarDecl->getTypeDecl()->getTypeTag());
        std::string_view lhsTypeName = typeStringMangleName(lhsTypeRef, lhsTypeTag);
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
        LLVMValueRef sameTypeResult = LLVMBuildCall(builder, addedIsSameTypeFunc, paramRefs, 2, "call");
        // creating branch condition using is_same_type() function result.
        LLVMValueRef brCondition[[maybe_unused]] = LLVMBuildIsNotNull(builder, sameTypeResult, "");
        pkgObj->addStringOffsetRelocationEntry(lhsTypeName.data(), lhsGep);

        LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, lhsOpName.c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
        LLVMBuildStore(builder, castLoad, lhsOpRef);
    } else if (funcObj->getLocalOrGlobalVariable(getLHS())->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {

        // struct first element original type
        LLVMValueRef origTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 0, origTypeChar);
        Variable *origVarDecl = funcObj->getLocalOrGlobalVariable(getLHS());
        assert(origVarDecl->getTypeDecl()->getTypeTag());
        TypeTag origTypeTag = TypeTag(origVarDecl->getTypeDecl()->getTypeTag());
        std::string_view origTypeName = typeStringMangleName(lhsTypeRef, origTypeTag);
        if (!strTable->contains(origTypeName))
            strTable->add(origTypeName);
        int tempRandNum1 = rand() % 1000 + 1;
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum1, 0);
        LLVMValueRef origStoreRef = LLVMBuildStore(builder, constValue, origTypeIdx);
        pkgObj->addStringOffsetRelocationEntry(origTypeName.data(), origStoreRef);
        // struct second element last type
        LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 1, lastTypeChar);
        Variable *lastTypeVarDecl = funcObj->getLocalOrGlobalVariable(rhsOp);
        assert(lastTypeVarDecl->getTypeDecl()->getTypeTag());
        TypeTag lastTypeTag = TypeTag(lastTypeVarDecl->getTypeDecl()->getTypeTag());
        std::string_view lastTypeName = typeStringMangleName(LLVMTypeOf(rhsOpRef), lastTypeTag);
        if (!strTable->contains(lastTypeName))
            strTable->add(lastTypeName.data());
        int tempRandNum2 = rand() % 1000 + 1;
        LLVMValueRef constValue1 = LLVMConstInt(LLVMInt32Type(), tempRandNum2, 0);
        LLVMValueRef lastStoreRef = LLVMBuildStore(builder, constValue1, lastTypeIdx);
        pkgObj->addStringOffsetRelocationEntry(lastTypeName.data(), lastStoreRef);
        // struct third element void pointer data.
        LLVMValueRef elePtr2 = LLVMBuildStructGEP(builder, lhsOpRef, 2, "data");
        LLVMValueRef bitCastRes1 = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMBuildStore(builder, bitCastRes1, elePtr2);
    } else {
        LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
}

LLVMValueRef TypeCastInsn::isSameType(LLVMModuleRef &modRef, LLVMValueRef lhsRef, LLVMValueRef rhsRef) {
    const char *isSameTypeChar = "is_same_type";
    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef(isSameTypeChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef *paramTypes = new LLVMTypeRef[2];
    paramTypes[0] = LLVMTypeOf(lhsRef);
    paramTypes[1] = LLVMTypeOf(rhsRef);
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt8Type(), paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, isSameTypeChar, funcType);
    getPackage()->addFunctionRef(isSameTypeChar, addedFuncRef);
    return addedFuncRef;
}

std::string_view TypeCastInsn::typeStringMangleName(LLVMTypeRef valType, TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT: {
        return "__I";
    }
    case TYPE_TAG_FLOAT: {
        return "__F";
    }
    case TYPE_TAG_CHAR_STRING: {
        return "__C";
    }
    case TYPE_TAG_STRING: {
        return "__S";
    }
    case TYPE_TAG_BOOLEAN: {
        return "__B";
    }
    case TYPE_TAG_ARRAY: {
        // TODO Need to add Size of the array.
        if (unwrap(valType)->getPointerElementType()->isIntegerTy())
            return "__AI";
        return "__A";
    }
    case TYPE_TAG_ANY: {
        return "__X";
    }
    default:
        return "";
    }
}

} // namespace nballerina
