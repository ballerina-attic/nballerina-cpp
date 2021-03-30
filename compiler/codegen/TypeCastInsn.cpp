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

TypeCastInsn::TypeCastInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &rhsOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), rhsOp(rhsOp) {}

void TypeCastInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    LLVMValueRef rhsOpRef = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMTypeRef lhsTypeRef = LLVMTypeOf(lhsOpRef);

    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    const auto &rhsVar = funcObj.getLocalOrGlobalVariable(rhsOp);
    const auto &lhsType = lhsVar.getType();
    auto lhsTypeTag = lhsType.getTypeTag();
    const auto &rhsType = rhsVar.getType();
    auto rhsTypeTag = rhsType.getTypeTag();

    const char *inherentTypeName = "inherentTypeName";

    if (rhsTypeTag == TYPE_TAG_ANY || rhsTypeTag == TYPE_TAG_UNION) {
        if (lhsTypeTag == TYPE_TAG_UNION || lhsTypeTag == TYPE_TAG_ANY) {
            LLVMValueRef rhsVarOpRef = funcObj.createTempVariable(rhsOp);
            LLVMBuildStore(builder, rhsVarOpRef, lhsOpRef);
            return;
        }
        // GEP of last type of smart pointer(original type of any variable(smart pointer))
        LLVMValueRef inherentTypeIdx = LLVMBuildStructGEP(builder, rhsOpRef, 0, inherentTypeName);
        LLVMValueRef inherentTypeLoad = LLVMBuildLoad(builder, inherentTypeIdx, "");
        LLVMValueRef sExt = LLVMBuildSExt(builder, inherentTypeLoad, LLVMInt64Type(), "");

        LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 1, "data");
        LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");
        LLVMValueRef strTblPtr = getPackageMutableRef().getStringBuilderTableGlobalPointer();
        LLVMValueRef strTblLoad = LLVMBuildLoad(builder, strTblPtr, "");
        LLVMValueRef gepOfStr = LLVMBuildInBoundsGEP(builder, strTblLoad, &sExt, 1, "");

        // get the mangled name of the lhs type and store it to string builder table.
        std::string_view lhsTypeName = typeStringMangleName(lhsType);
        getPackageMutableRef().addToStrTable(lhsTypeName);
        int tempRandNum = std::rand() % 1000 + 1;
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum, 0);
        LLVMValueRef lhsGep = LLVMBuildInBoundsGEP(builder, strTblLoad, &constValue, 1, "");
        // call is_same_type rust function to check LHS and RHS type are same or not.
        LLVMValueRef addedIsSameTypeFunc = getIsSameTypeDeclaration(modRef, lhsGep, gepOfStr);
        LLVMValueRef paramRefs[] = {lhsGep, gepOfStr};
        LLVMValueRef sameTypeResult = LLVMBuildCall(builder, addedIsSameTypeFunc, paramRefs, 2, "call");
        // creating branch condition using is_same_type() function result.
        [[maybe_unused]] LLVMValueRef brCondition = LLVMBuildIsNotNull(builder, sameTypeResult, "");
        getPackageMutableRef().addStringOffsetRelocationEntry(lhsTypeName.data(), lhsGep);

        LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, getLhsOperand().getName().c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
        LLVMBuildStore(builder, castLoad, lhsOpRef);

    } else if (lhsTypeTag == TYPE_TAG_ANY || lhsTypeTag == TYPE_TAG_UNION) {

        // struct first element original type
        LLVMValueRef inherentTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 0, inherentTypeName);
        std::string_view rhsTypeName = typeStringMangleName(rhsType);
        getPackageMutableRef().addToStrTable(rhsTypeName);
        int tempRandNum1 = std::rand() % 1000 + 1;
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum1, 0);
        LLVMValueRef lhsTypeStoreRef = LLVMBuildStore(builder, constValue, inherentTypeIdx);
        getPackageMutableRef().addStringOffsetRelocationEntry(rhsTypeName.data(), lhsTypeStoreRef);
        // struct second element void pointer data.
        LLVMValueRef elePtr2 = LLVMBuildStructGEP(builder, lhsOpRef, 1, "data");
        if (isBoxValueSupport(rhsTypeTag)) {
            LLVMValueRef rhsTempOpRef = funcObj.createTempVariable(rhsOp);
            LLVMValueRef boxValFunc = generateBoxValueFunc(modRef, LLVMTypeOf(rhsTempOpRef), rhsTypeTag);
            rhsOpRef = LLVMBuildCall(builder, boxValFunc, &rhsTempOpRef, 1, "call");
        }
        LLVMValueRef bitCastRes = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMBuildStore(builder, bitCastRes, elePtr2);
    } else {
        LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
}

LLVMValueRef TypeCastInsn::generateBoxValueFunc(LLVMModuleRef &modRef, LLVMTypeRef paramTypeRef, TypeTag typeTag) {
    std::string functionName = "box_bal_";
    functionName += Type::getNameOfType(typeTag);
    LLVMValueRef boxValFuncRef = getPackageRef().getFunctionRef(functionName);
    if (boxValFuncRef != nullptr) {
        return boxValFuncRef;
    }
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(paramTypeRef, 0), &paramTypeRef, 1, 0);
    boxValFuncRef = LLVMAddFunction(modRef, functionName.c_str(), funcType);
    getPackageMutableRef().addFunctionRef(functionName, boxValFuncRef);
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

LLVMValueRef TypeCastInsn::getIsSameTypeDeclaration(LLVMModuleRef &modRef, LLVMValueRef lhsRef, LLVMValueRef rhsRef) {
    const char *isSameTypeChar = "is_same_type";
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(isSameTypeChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMTypeOf(lhsRef), LLVMTypeOf(rhsRef)};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt8Type(), paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, isSameTypeChar, funcType);
    getPackageMutableRef().addFunctionRef(isSameTypeChar, addedFuncRef);
    return addedFuncRef;
}

std::string_view TypeCastInsn::typeStringMangleName(const Type &type) {
    switch (type.getTypeTag()) {
    case TYPE_TAG_INT: {
        return "__I";
    }
    case TYPE_TAG_FLOAT: {
        return "__F";
    }
    case TYPE_TAG_STRING: {
        return "__S";
    }
    case TYPE_TAG_BOOLEAN: {
        return "__B";
    }
    case TYPE_TAG_ARRAY: {
        // TODO add array type and size
        return "__A";
    }
    case TYPE_TAG_ANY: {
        return "__X";
    }
    case TYPE_TAG_NIL: {
        return "__N";
    }
    case TYPE_TAG_MAP: {
        TypeTag memberTypeTag = type.getMemberTypeTag();
        switch (memberTypeTag) {
        case TYPE_TAG_INT:
            return "__MI";
        default:
            return "__M";
        }
    }
    default:
        return "";
    }
}

} // namespace nballerina
