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
#include "CodeGenUtils.h"
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

void ArrayInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    auto *sizeOpValueRef = funcObj.createTempVariable(sizeOp, module, builder);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    const auto &lhsVar = getFunctionRef().getLocalOrGlobalVariable(getLhsOperand());
    TypeTag memberTypeTag = lhsVar.getType().getMemberTypeTag();
    auto arrayInitFunc = CodeGenUtils::getArrayInitFunc(module, memberTypeTag);
    auto *newArrayRef = builder.CreateCall(arrayInitFunc, llvm::ArrayRef<llvm::Value *>({sizeOpValueRef}));
    builder.CreateStore(newArrayRef, lhsOpRef);
}

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &KOp, const Operand &ROp)
    : NonTerminatorInsn(lhs, currentBB), keyOp(KOp), rhsOp(ROp) {}

void ArrayLoadInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &lhsOpTypeTag = funcObj.getLocalOrGlobalVariable(getLhsOperand()).getType().getTypeTag();
    auto ArrayLoadFunc = CodeGenUtils::getArrayLoadFunc(module, lhsOpTypeTag);

    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *rhsOpTempRef = funcObj.createTempVariable(rhsOp, module, builder);
    auto *bitCastRhsOpTempRef = builder.CreateBitCast(
        rhsOpTempRef, builder.getInt8PtrTy(), "");
    auto *keyOpTempRef = funcObj.createTempVariable(keyOp, module, builder);
    auto *valueInArrayPointer =
        builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({bitCastRhsOpTempRef, keyOpTempRef}));

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

void ArrayStoreInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &rhsOpTypeTag = funcObj.getLocalOrGlobalVariable(rhsOp).getType().getTypeTag();
    auto ArrayLoadFunc = CodeGenUtils::getArrayStoreFunc(module, rhsOpTypeTag);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *memVal = Type::isSmartStructType(rhsOpTypeTag) ? funcObj.getLLVMLocalOrGlobalVar(rhsOp, module)
                                                         : funcObj.createTempVariable(rhsOp, module, builder);
    auto *lhsOpTempRef = builder.CreateLoad(lhsOpRef);
    auto *bitCastLhsOpTempRef = builder.CreateBitCast(
        lhsOpTempRef, builder.getInt8PtrTy(), "");
    auto *keyOpTempRef = funcObj.createTempVariable(keyOp, module, builder);

    builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({bitCastLhsOpTempRef, keyOpTempRef, memVal}));
}

} // namespace nballerina
