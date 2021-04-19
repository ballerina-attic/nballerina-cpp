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

#include "ArrayInstructions.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include <string>

using namespace std;

namespace nballerina {

// New Array Instruction
ArrayInsn::ArrayInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &sizeOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), sizeOp(sizeOp) {}

LLVMValueRef ArrayInsn::getArrayInitDeclaration(LLVMModuleRef &modRef) {
    const auto &lhsVar = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand());
    const auto &arrayType = lhsVar.getType();
    TypeTag memberTypeTag = arrayType.getMemberTypeTag();
    const auto arrayTypeFuncName = "array_init_" + Type::getNameOfType(memberTypeTag);
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(arrayTypeFuncName);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes = LLVMInt64Type();
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt8Type(), 0), &paramTypes, 1, 0);
    addedFuncRef = LLVMAddFunction(modRef, arrayTypeFuncName.c_str(), funcType);
    getPackageMutableRef().addFunctionRef(arrayTypeFuncName, addedFuncRef);
    return addedFuncRef;
}

void ArrayInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    LLVMValueRef sizeOpValueRef = funcObj.createTempVariable(sizeOp);
    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef arrayInitFunc = getArrayInitDeclaration(modRef);
    LLVMValueRef newArrayRef = LLVMBuildCall(builder, arrayInitFunc, &sizeOpValueRef, 1, "");

    LLVMBuildStore(builder, newArrayRef, lhsOpRef);
}

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &KOp,
                             const Operand &ROp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef, TypeTag lhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_load_" + Type::getNameOfType(lhsOpTypeTag);
    const auto &lhsType = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand()).getType();
    LLVMTypeRef funcRetType;
    if (Type::isSmartStructType(lhsType.getTypeTag())) {
        funcRetType = LLVMPointerType(getPackageRef().getLLVMTypeOfType(lhsType), 0);
    } else {
        funcRetType = getPackageRef().getLLVMTypeOfType(lhsType);
    }
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(arrayTypeFuncName);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt64Type()};
    LLVMTypeRef funcType = LLVMFunctionType(funcRetType, paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, arrayTypeFuncName.c_str(), funcType);
    getPackageMutableRef().addFunctionRef(arrayTypeFuncName, addedFuncRef);
    return addedFuncRef;
}

void ArrayLoadInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    const auto &lhsOpTypeTag = funcObj.getLocalOrGlobalVariable(getLhsOperand()).getType().getTypeTag();
    LLVMValueRef ArrayLoadFunc = getArrayLoadDeclaration(modRef, lhsOpTypeTag);

    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef rhsOpRef = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef sizeOpValueRef[] = {LLVMBuildLoad(builder, rhsOpRef, ""), funcObj.createTempVariable(keyOp)};
    LLVMValueRef valueInArrayPointer = LLVMBuildCall(builder, ArrayLoadFunc, sizeOpValueRef, 2, "");
    if (!Type::isSmartStructType(lhsOpTypeTag)) {
        LLVMBuildStore(builder, valueInArrayPointer, lhsOpRef);
        return;
    }
    LLVMValueRef smtPtrArrElement = LLVMBuildLoad(builder, valueInArrayPointer, "");
    LLVMBuildStore(builder, smtPtrArrElement, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &KOp,
                               const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef, TypeTag rhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_store_" + Type::getNameOfType(rhsOpTypeTag);
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(arrayTypeFuncName);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef memType;
    if (Type::isSmartStructType(rhsOpTypeTag)) {
        memType = LLVMPointerType(getPackageRef().getLLVMTypeOfType(rhsOpTypeTag), 0);
    } else {
        memType = getPackageRef().getLLVMTypeOfType(rhsOpTypeTag);
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0),  LLVMInt64Type(), memType};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    addedFuncRef = LLVMAddFunction(modRef, arrayTypeFuncName.c_str(), funcType);
    getPackageMutableRef().addFunctionRef(arrayTypeFuncName, addedFuncRef);
    return addedFuncRef;
}

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    const auto &rhsOpTypeTag = funcObj.getLocalOrGlobalVariable(rhsOp).getType().getTypeTag();
    LLVMValueRef ArrayLoadFunc = getArrayStoreDeclaration(modRef, rhsOpTypeTag);

    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef memVal;
    if (Type::isSmartStructType(rhsOpTypeTag)) {
        memVal = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    } else {
        memVal = funcObj.createTempVariable(rhsOp);
    }
    LLVMValueRef argOpValueRef[] = {LLVMBuildLoad(builder, lhsOpRef, ""), funcObj.createTempVariable(keyOp), memVal};
    LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
