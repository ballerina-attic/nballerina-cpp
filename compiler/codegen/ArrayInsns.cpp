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
#include "llvm-c/Core.h"
#include <string>

using namespace std;

namespace nballerina {

// New Array Instruction
ArrayInsn::ArrayInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &sizeOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), sizeOp(sizeOp) {}

LLVMValueRef ArrayInsn::getArrayInitDeclaration(LLVMModuleRef &modRef) {
    const char *newIntArrayChar = "new_int_array";
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(newIntArrayChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes = LLVMInt32Type();
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt8Type(), 0), &paramTypes, 1, 0);
    addedFuncRef = LLVMAddFunction(modRef, newIntArrayChar, funcType);
    getPackageMutableRef().addFunctionRef(newIntArrayChar, addedFuncRef);
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
ArrayLoadInsn::ArrayLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp,
                             const Operand &ROp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef) {
    const char *intArrayLoadChar = "int_array_load";
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(intArrayLoadChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt32Type()};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt32Type(), 0), paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, intArrayLoadChar, funcType);
    getPackageMutableRef().addFunctionRef(intArrayLoadChar, addedFuncRef);
    return addedFuncRef;
}

void ArrayLoadInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = getArrayLoadDeclaration(modRef);

    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef rhsOpRef = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef sizeOpValueRef[] = {LLVMBuildLoad(builder, rhsOpRef, ""), funcObj.createTempVariable(keyOp)};
    LLVMValueRef valueInArrayPointer = LLVMBuildCall(builder, ArrayLoadFunc, sizeOpValueRef, 2, "");
    LLVMValueRef ArrayTempVal = LLVMBuildLoad(builder, valueInArrayPointer, "");
    LLVMBuildStore(builder, ArrayTempVal, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp,
                               const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef) {
    const char *intArrayStoreChar = "int_array_store";
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(intArrayStoreChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt32Type(),
                                LLVMPointerType(LLVMInt32Type(), 0)};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    addedFuncRef = LLVMAddFunction(modRef, intArrayStoreChar, funcType);
    getPackageMutableRef().addFunctionRef(intArrayStoreChar, addedFuncRef);
    return addedFuncRef;
}

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = getArrayStoreDeclaration(modRef);

    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef argOpValueRef[] = {LLVMBuildLoad(builder, lhsOpRef, ""), funcObj.createTempVariable(keyOp),
                                    funcObj.getLLVMLocalOrGlobalVar(rhsOp)};
    LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
