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
#include <string>

using namespace std;

namespace nballerina {

// New Array Instruction
ArrayInsn::ArrayInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &sizeOp)
    : NonTerminatorInsn(lhs, currentBB), sizeOp(sizeOp) {}

llvm::FunctionCallee ArrayInsn::getArrayInitDeclaration(llvm::Module &module) {
    const auto &lhsVar = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand());
    TypeTag memberTypeTag = lhsVar.getType().getMemberTypeTag();
    const auto arrayTypeFuncName = "array_init_" + Type::getNameOfType(memberTypeTag);
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(module.getContext())}), false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

void ArrayInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    auto *sizeOpValueRef = funcObj.createTempVariable(sizeOp, module, builder);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto arrayInitFunc = getArrayInitDeclaration(module);
    auto *newArrayRef = builder.CreateCall(arrayInitFunc, llvm::ArrayRef<llvm::Value *>({sizeOpValueRef}));
    builder.CreateStore(newArrayRef, lhsOpRef);
}

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &KOp, const Operand &ROp)
    : NonTerminatorInsn(lhs, currentBB), keyOp(KOp), rhsOp(ROp) {}

llvm::FunctionCallee ArrayLoadInsn::getArrayLoadDeclaration(llvm::Module &module, TypeTag lhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_load_" + Type::getNameOfType(lhsOpTypeTag);
    const auto &lhsType = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand()).getType();

    llvm::Type *funcRetType = Type::isSmartStructType(lhsType.getTypeTag())
                                  ? llvm::PointerType::get(getPackageRef().getLLVMTypeOfType(lhsType, module), 0)
                                  : getPackageRef().getLLVMTypeOfType(lhsType, module);
    auto *funcType =
        llvm::FunctionType::get(funcRetType,
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt64Ty(module.getContext())}),
                                false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

void ArrayLoadInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &lhsOpTypeTag = funcObj.getLocalOrGlobalVariable(getLhsOperand()).getType().getTypeTag();
    auto ArrayLoadFunc = getArrayLoadDeclaration(module, lhsOpTypeTag);

    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *rhsOpTempRef = funcObj.createTempVariable(rhsOp, module, builder);
    auto *keyOpTempRef = funcObj.createTempVariable(keyOp, module, builder);
    auto *valueInArrayPointer =
        builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({rhsOpTempRef, keyOpTempRef}));

    if (!Type::isSmartStructType(lhsOpTypeTag)) {
        builder.CreateStore(valueInArrayPointer, lhsOpRef);
        return;
    }
    auto *smtPtrArrElement = builder.CreateLoad(valueInArrayPointer);
    builder.CreateStore(smtPtrArrElement, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &KOp, const Operand &rOp)
    : NonTerminatorInsn(lhs, currentBB), keyOp(KOp), rhsOp(rOp) {}

llvm::FunctionCallee ArrayStoreInsn::getArrayStoreDeclaration(llvm::Module &module, TypeTag rhsOpTypeTag) {
    const auto arrayTypeFuncName = "array_store_" + Type::getNameOfType(rhsOpTypeTag);
    llvm::Type *memType = Type::isSmartStructType(rhsOpTypeTag)
                              ? llvm::PointerType::get(getPackageRef().getLLVMTypeOfType(rhsOpTypeTag, module), 0)
                              : getPackageRef().getLLVMTypeOfType(rhsOpTypeTag, module);
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt64Ty(module.getContext()), memType}),
                                false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

void ArrayStoreInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &rhsOpTypeTag = funcObj.getLocalOrGlobalVariable(rhsOp).getType().getTypeTag();
    auto ArrayLoadFunc = getArrayStoreDeclaration(module, rhsOpTypeTag);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *memVal = Type::isSmartStructType(rhsOpTypeTag) ? funcObj.getLLVMLocalOrGlobalVar(rhsOp, module)
                                                         : funcObj.createTempVariable(rhsOp, module, builder);
    auto *lhsOpTempRef = builder.CreateLoad(lhsOpRef);
    auto *keyOpTempRef = funcObj.createTempVariable(keyOp, module, builder);

    builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({lhsOpTempRef, keyOpTempRef, memVal}));
}

} // namespace nballerina
