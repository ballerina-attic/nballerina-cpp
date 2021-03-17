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
ArrayInsn::ArrayInsn(Operand *lOp, BasicBlock *currentBB, Operand *sOp, [[maybe_unused]] Type *tDecl)
    : NonTerminatorInsn(lOp, currentBB), sizeOp(sOp) {}

LLVMValueRef ArrayInsn::getArrayInitDeclaration(LLVMModuleRef &modRef) {
    const char *newIntArrayChar = "new_int_array";
    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef(newIntArrayChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef *paramTypes = new LLVMTypeRef[1];
    paramTypes[0] = LLVMInt32Type();
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt8Type(), 0), paramTypes, 1, 0);
    addedFuncRef = LLVMAddFunction(modRef, newIntArrayChar, funcType);
    getPackage()->addFunctionRef(newIntArrayChar, addedFuncRef);
    return addedFuncRef;
}

void ArrayInsn::translate(LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef *sizeOpValueRef = new LLVMValueRef[1];
    sizeOpValueRef[0] = funcObj->getTempLocalVariable(sizeOp);

    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMValueRef arrayInitFunc = getArrayInitDeclaration(modRef);
    LLVMValueRef newArrayRef = LLVMBuildCall(builder, arrayInitFunc, sizeOpValueRef, 1, "");

    LLVMBuildStore(builder, newArrayRef, lhsOpRef);
}

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(Operand *lOp, BasicBlock *currentBB, [[maybe_unused]] bool optionalFieldAccess,
                             [[maybe_unused]] bool fillingRead, Operand *KOp, Operand *ROp)
    : NonTerminatorInsn(lOp, currentBB), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef) {
    const char *intArrayLoadChar = "int_array_load";
    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef(intArrayLoadChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef *paramTypes = new LLVMTypeRef[2];
    paramTypes[0] = LLVMPointerType(LLVMInt8Type(), 0);
    paramTypes[1] = LLVMInt32Type();
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt32Type(), 0), paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, intArrayLoadChar, funcType);
    getPackage()->addFunctionRef(intArrayLoadChar, addedFuncRef);
    return addedFuncRef;
}

void ArrayLoadInsn::translate(LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = getArrayLoadDeclaration(modRef);

    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef *sizeOpValueRef = new LLVMValueRef[2];
    sizeOpValueRef[0] = LLVMBuildLoad(builder, rhsOpRef, "");
    sizeOpValueRef[1] = funcObj->getTempLocalVariable(keyOp);
    LLVMValueRef valueInArrayPointer = LLVMBuildCall(builder, ArrayLoadFunc, sizeOpValueRef, 2, "");
    LLVMValueRef ArrayTempVal = LLVMBuildLoad(builder, valueInArrayPointer, "");
    LLVMBuildStore(builder, ArrayTempVal, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(Operand *lOp, BasicBlock *currentBB, Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(lOp, currentBB), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef) {
    const char *intArrayStoreChar = "int_array_store";
    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef(intArrayStoreChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef *paramTypes = new LLVMTypeRef[3];
    paramTypes[0] = LLVMPointerType(LLVMInt8Type(), 0);
    paramTypes[1] = LLVMInt32Type();
    paramTypes[2] = LLVMPointerType(LLVMInt32Type(), 0);
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    addedFuncRef = LLVMAddFunction(modRef, intArrayStoreChar, funcType);
    getPackage()->addFunctionRef(intArrayStoreChar, addedFuncRef);
    return addedFuncRef;
}

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = getArrayStoreDeclaration(modRef);

    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
    argOpValueRef[0] = LLVMBuildLoad(builder, lhsOpRef, "");
    argOpValueRef[1] = funcObj->getTempLocalVariable(keyOp);
    argOpValueRef[2] = funcObj->getLLVMLocalOrGlobalVar(rhsOp);

    LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
