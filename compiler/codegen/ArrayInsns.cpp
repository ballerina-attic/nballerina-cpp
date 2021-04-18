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

    auto *module = llvm::unwrap(modRef);
    auto *functionRef = module->getFunction(arrayTypeFuncName);
    if (functionRef != nullptr) {
        return llvm::wrap(functionRef);
    }

    LLVMTypeRef paramTypes = LLVMInt64Type();
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt8Type(), 0), &paramTypes, 1, 0);
    return LLVMAddFunction(modRef, arrayTypeFuncName.c_str(), funcType);
}

void ArrayInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    auto modRef = llvm::wrap(&module);
    LLVMValueRef sizeOpValueRef = llvm::wrap(funcObj.createTempVariable(sizeOp, module, builder));
    LLVMValueRef lhsOpRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMValueRef arrayInitFunc = getArrayInitDeclaration(modRef);
    LLVMValueRef newArrayRef = LLVMBuildCall(llvm::wrap(&builder), arrayInitFunc, &sizeOpValueRef, 1, "");

    LLVMBuildStore(llvm::wrap(&builder), newArrayRef, lhsOpRef);
}

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &KOp,
                             const Operand &ROp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(ROp) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef, TypeTag lhsOpTypeTag) {
    auto *module = llvm::unwrap(modRef);
    const auto arrayTypeFuncName = "array_load_" + Type::getNameOfType(lhsOpTypeTag);
    const auto &lhsType = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand()).getType();

    llvm::Type *funcRetType = Type::isSmartStructType(lhsType.getTypeTag())
                                  ? llvm::PointerType::get(getPackageRef().getLLVMTypeOfType(lhsType, *module), 0)
                                  : getPackageRef().getLLVMTypeOfType(lhsType, *module);

    auto *functionRef = module->getFunction(arrayTypeFuncName);
    if (functionRef != nullptr) {
        return llvm::wrap(functionRef);
    }

    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt64Type()};
    LLVMTypeRef funcType = LLVMFunctionType(llvm::wrap(funcRetType), paramTypes, 2, 0);
    return LLVMAddFunction(modRef, arrayTypeFuncName.c_str(), funcType);
}

void ArrayLoadInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &lhsOpTypeTag = funcObj.getLocalOrGlobalVariable(getLhsOperand()).getType().getTypeTag();
    auto modRef = llvm::wrap(&module);
    LLVMValueRef ArrayLoadFunc = getArrayLoadDeclaration(modRef, lhsOpTypeTag);

    LLVMValueRef lhsOpRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMValueRef rhsOpRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(rhsOp, module));
    LLVMValueRef sizeOpValueRef[] = {LLVMBuildLoad(llvm::wrap(&builder), rhsOpRef, ""),
                                     llvm::wrap(funcObj.createTempVariable(keyOp, module, builder))};
    LLVMValueRef valueInArrayPointer = LLVMBuildCall(llvm::wrap(&builder), ArrayLoadFunc, sizeOpValueRef, 2, "");
    if (!Type::isSmartStructType(lhsOpTypeTag)) {
        LLVMBuildStore(llvm::wrap(&builder), valueInArrayPointer, lhsOpRef);
        return;
    }
    LLVMValueRef smtPtrArrElement = LLVMBuildLoad(llvm::wrap(&builder), valueInArrayPointer, "");
    LLVMBuildStore(llvm::wrap(&builder), smtPtrArrElement, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &KOp,
                               const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef, TypeTag rhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_store_" + Type::getNameOfType(rhsOpTypeTag);
    auto *module = llvm::unwrap(modRef);
    auto *functionRef = module->getFunction(arrayTypeFuncName);
    if (functionRef != nullptr) {
        return llvm::wrap(functionRef);
    }

    llvm::Type *memType = Type::isSmartStructType(rhsOpTypeTag)
                              ? llvm::PointerType::get(getPackageRef().getLLVMTypeOfType(rhsOpTypeTag, *module), 0)
                              : getPackageRef().getLLVMTypeOfType(rhsOpTypeTag, *module);

    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt64Type(), llvm::wrap(memType)};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    return LLVMAddFunction(modRef, arrayTypeFuncName.c_str(), funcType);
}

void ArrayStoreInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &rhsOpTypeTag = funcObj.getLocalOrGlobalVariable(rhsOp).getType().getTypeTag();
    auto modRef = llvm::wrap(&module);
    LLVMValueRef ArrayLoadFunc = getArrayStoreDeclaration(modRef, rhsOpTypeTag);

    LLVMValueRef lhsOpRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMValueRef memVal;
    if (Type::isSmartStructType(rhsOpTypeTag)) {
        memVal = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(rhsOp, module));
    } else {
        memVal = llvm::wrap(funcObj.createTempVariable(rhsOp, module, builder));
    }
    LLVMValueRef argOpValueRef[] = {LLVMBuildLoad(llvm::wrap(&builder), lhsOpRef, ""),
                                    llvm::wrap(funcObj.createTempVariable(keyOp, module, builder)), memVal};
    LLVMBuildCall(llvm::wrap(&builder), ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
