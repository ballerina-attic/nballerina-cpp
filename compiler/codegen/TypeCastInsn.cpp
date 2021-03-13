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
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMTypeRef lhsTypeRef = LLVMTypeOf(lhsOpRef);

    TypeTag rhsTypeTag = funcObj->getLocalOrGlobalVariable(rhsOp)->getTypeDecl()->getTypeTag();
    TypeTag lhsTypeTag = funcObj->getLocalOrGlobalVariable(getLHS())->getTypeDecl()->getTypeTag();
    StringTableBuilder *strTable = pkgObj->getStrTableBuilder();
    if (rhsTypeTag == TYPE_TAG_ANY || rhsTypeTag == TYPE_TAG_UNION) {
        if (lhsTypeTag == TYPE_TAG_UNION || lhsTypeTag == TYPE_TAG_ANY) {
            LLVMValueRef rhsVarOpRef = funcObj->getTempLocalVariable(rhsOp);
            LLVMBuildStore(builder, rhsVarOpRef, lhsOpRef);
            return;
        }
        LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, rhsOpRef, 0, "inherentTypeIdx");
        LLVMValueRef lastTypeLoad = LLVMBuildLoad(builder, lastTypeIdx, "");
        LLVMValueRef sExt = LLVMBuildSExt(builder, lastTypeLoad, LLVMInt64Type(), "");

        LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 1, "data");
        LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");
        LLVMValueRef strTblPtr = pkgObj->getStringBuilderTableGlobalPointer();
        LLVMValueRef strTblLoad = LLVMBuildLoad(builder, strTblPtr, "");
        LLVMValueRef *sizeOpValueRef = new LLVMValueRef[1];
        sizeOpValueRef[0] = sExt;
        LLVMValueRef gepOfStr = LLVMBuildInBoundsGEP(builder, strTblLoad, sizeOpValueRef, 1, "");
        // get the mangled name of the lhs type and store it to string
        // builder table.
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
        LLVMValueRef brCondition [[maybe_unused]] = LLVMBuildIsNotNull(builder, sameTypeResult, "");
        pkgObj->addStringOffsetRelocationEntry(lhsTypeName.data(), lhsGep);

        LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, lhsOpName.c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
        LLVMBuildStore(builder, castLoad, lhsOpRef);
    } else if (lhsTypeTag == TYPE_TAG_UNION || lhsTypeTag == TYPE_TAG_ANY) {
        LLVMValueRef inherentTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 0, "inherentTypeIdx");
        std::string_view rhsTypeName = typeStringMangleName(lhsTypeRef, rhsTypeTag);
        if (!strTable->contains(rhsTypeName))
            strTable->add(rhsTypeName);
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), -1, 0);
        LLVMValueRef lhsTypeStoreRef = LLVMBuildStore(builder, constValue, inherentTypeIdx);
        pkgObj->addStringOffsetRelocationEntry(rhsTypeName.data(), lhsTypeStoreRef);
        LLVMValueRef data = LLVMBuildStructGEP(builder, lhsOpRef, 1, "data");
        if (isBoxValueSupport(rhsTypeTag)) {
            LLVMValueRef rhsTempOpRef = funcObj->getTempLocalVariable(rhsOp);
            LLVMValueRef *paramRefs = new LLVMValueRef[1];
            paramRefs[0] = rhsTempOpRef;
            LLVMValueRef boxValFunc = generateBoxValueFunc(modRef, LLVMTypeOf(rhsTempOpRef), rhsTypeTag);
            rhsOpRef = LLVMBuildCall(builder, boxValFunc, paramRefs, 1, "call");
        }
        LLVMValueRef bitCast = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMBuildStore(builder, bitCast, data);
    } else {
        LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
}

LLVMValueRef TypeCastInsn::generateBoxValueFunc(LLVMModuleRef &modRef, LLVMTypeRef paramTypeRef, TypeTag typeTag) {
    std::string functionName = "box_bal_";
    functionName += Type::getNameOfType(typeTag);
    LLVMValueRef boxValFuncRef = getPackage()->getFunctionRef(functionName);
    if (boxValFuncRef != nullptr) {
        return boxValFuncRef;
    }
    LLVMTypeRef *paramTypes = new LLVMTypeRef[1];
    paramTypes[0] = paramTypeRef;
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(paramTypeRef, 0), paramTypes, 1, 0);
    boxValFuncRef = LLVMAddFunction(modRef, functionName.c_str(), funcType);
    getPackage()->addFunctionRef(functionName, boxValFuncRef);
    return boxValFuncRef;
}

bool TypeCastInsn::isBoxValueSupport(TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT:
    case TYPE_TAG_FLOAT:
    case TYPE_TAG_BOOLEAN:
        return true;
    default:
        return false;
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
