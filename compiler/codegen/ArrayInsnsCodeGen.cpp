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

#include "bir/ArrayInstructions.h"
#include "bir/Function.h"
#include "bir/Types.h"
#include "bir/Variable.h"
#include "codegen/CodeGenUtils.h"
#include "codegen/NonTerminatorInsnCodeGen.h"
#include <string>

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(ArrayInsn &obj, llvm::IRBuilder<> &builder) {
    auto *sizeOpValueRef = functionGenerator.createTempVal(obj.sizeOp, builder);
    auto *lhsOpRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    const auto &lhsVar = obj.getFunctionRef().getLocalOrGlobalVariable(obj.lhsOp);
    TypeTag memberTypeTag = lhsVar.getType().getMemberTypeTag();
    auto arrayInitFunc = CodeGenUtils::getArrayInitFunc(moduleGenerator.getModule(), memberTypeTag);
    auto *newArrayRef = builder.CreateCall(arrayInitFunc, llvm::ArrayRef<llvm::Value *>({sizeOpValueRef}));
    builder.CreateStore(newArrayRef, lhsOpRef);
}

void NonTerminatorInsnCodeGen::visit(ArrayLoadInsn &obj, llvm::IRBuilder<> &builder) {
    const auto &lhsOpTypeTag = obj.getFunctionRef().getLocalOrGlobalVariable(obj.lhsOp).getType().getTypeTag();
    auto ArrayLoadFunc = CodeGenUtils::getArrayLoadFunc(moduleGenerator.getModule(), lhsOpTypeTag);

    auto *lhsOpRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    auto *rhsOpTempRef = functionGenerator.createTempVal(obj.rhsOp, builder);
    auto *keyOpTempRef = functionGenerator.createTempVal(obj.keyOp, builder);
    auto *valueInArrayPointer =
        builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({rhsOpTempRef, keyOpTempRef}));

    if (!Type::isBalValueType(lhsOpTypeTag)) {
        builder.CreateStore(valueInArrayPointer, lhsOpRef);
        return;
    }
    auto *smtPtrArrElement = builder.CreateLoad(valueInArrayPointer);
    builder.CreateStore(smtPtrArrElement, lhsOpRef);
}

void NonTerminatorInsnCodeGen::visit(ArrayStoreInsn &obj, llvm::IRBuilder<> &builder) {
    const auto &rhsOpTypeTag = obj.getFunctionRef().getLocalOrGlobalVariable(obj.rhsOp).getType().getTypeTag();
    auto ArrayLoadFunc = CodeGenUtils::getArrayStoreFunc(moduleGenerator.getModule(), rhsOpTypeTag);
    auto *lhsOpRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    auto *memVal = Type::isBalValueType(rhsOpTypeTag) ? functionGenerator.getLocalOrGlobalVal(obj.rhsOp)
                                                      : functionGenerator.createTempVal(obj.rhsOp, builder);
    auto *lhsOpTempRef = builder.CreateLoad(lhsOpRef);
    auto *keyOpTempRef = functionGenerator.createTempVal(obj.keyOp, builder);

    builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({lhsOpTempRef, keyOpTempRef, memVal}));
}

} // namespace nballerina
