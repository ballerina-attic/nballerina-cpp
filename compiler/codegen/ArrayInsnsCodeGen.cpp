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
#include "NonTerminatorInsnCodeGen.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include <string>

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(ArrayInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    auto *sizeOpValueRef = funcObj.createTempVariable(obj.sizeOp, module, builder);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(obj.getLhsOperand(), module);
    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(obj.getLhsOperand());
    TypeTag memberTypeTag = lhsVar.getType().getMemberTypeTag();
    auto arrayInitFunc = CodeGenUtils::getArrayInitFunc(module, memberTypeTag);
    auto *newArrayRef = builder.CreateCall(arrayInitFunc, llvm::ArrayRef<llvm::Value *>({sizeOpValueRef}));
    builder.CreateStore(newArrayRef, lhsOpRef);
}

void NonTerminatorInsnCodeGen::visit(ArrayLoadInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    const auto &lhsOpTypeTag = funcObj.getLocalOrGlobalVariable(obj.getLhsOperand()).getType().getTypeTag();
    auto ArrayLoadFunc = CodeGenUtils::getArrayLoadFunc(module, lhsOpTypeTag);

    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(obj.getLhsOperand(), module);
    auto *rhsOpTempRef = funcObj.createTempVariable(obj.rhsOp, module, builder);
    auto *keyOpTempRef = funcObj.createTempVariable(obj.keyOp, module, builder);
    auto *valueInArrayPointer =
        builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({rhsOpTempRef, keyOpTempRef}));

    if (!Type::isSmartStructType(lhsOpTypeTag)) {
        builder.CreateStore(valueInArrayPointer, lhsOpRef);
        return;
    }
    auto *smtPtrArrElement = builder.CreateLoad(valueInArrayPointer);
    builder.CreateStore(smtPtrArrElement, lhsOpRef);
}

void NonTerminatorInsnCodeGen::visit(ArrayStoreInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    const auto &rhsOpTypeTag = funcObj.getLocalOrGlobalVariable(obj.rhsOp).getType().getTypeTag();
    auto ArrayLoadFunc = CodeGenUtils::getArrayStoreFunc(module, rhsOpTypeTag);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(obj.getLhsOperand(), module);
    auto *memVal = Type::isSmartStructType(rhsOpTypeTag) ? funcObj.getLLVMLocalOrGlobalVar(obj.rhsOp, module)
                                                         : funcObj.createTempVariable(obj.rhsOp, module, builder);
    auto *lhsOpTempRef = builder.CreateLoad(lhsOpRef);
    auto *keyOpTempRef = funcObj.createTempVariable(obj.keyOp, module, builder);

    builder.CreateCall(ArrayLoadFunc, llvm::ArrayRef<llvm::Value *>({lhsOpTempRef, keyOpTempRef, memVal}));
}

} // namespace nballerina
