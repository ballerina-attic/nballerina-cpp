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
ArrayInsn::ArrayInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &sizeOp)
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
    LLVMTypeRef paramTypes = LLVMInt32Type();
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
ArrayLoadInsn::ArrayLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp,
                             const Operand &ROp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef, TypeTag lhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_load_" + Type::getNameOfType(lhsOpTypeTag);
    const auto &lhsType = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand()).getType();
    LLVMTypeRef funcRetType = LLVMPointerType(getPackageRef().getLLVMTypeOfType(lhsType), 0);
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(arrayTypeFuncName);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt32Type()};
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
    LLVMValueRef ArrayLoadVal = LLVMBuildLoad(builder, valueInArrayPointer, "");
    LLVMBuildStore(builder, ArrayLoadVal, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp,
                               const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef, TypeTag rhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_store_" + Type::getNameOfType(rhsOpTypeTag);
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(arrayTypeFuncName);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt32Type(),
                                LLVMTypeOf(getFunctionRef().getLLVMLocalVar(rhsOp.getName()))};
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
    LLVMValueRef argOpValueRef[] = {LLVMBuildLoad(builder, lhsOpRef, ""), funcObj.createTempVariable(keyOp),
                                    funcObj.getLLVMLocalOrGlobalVar(rhsOp)};
    LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
