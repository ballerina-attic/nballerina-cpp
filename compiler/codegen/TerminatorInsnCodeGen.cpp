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

#include "TerminatorInsnCodeGen.h"
#include "BasicBlock.h"
#include "ConditionBrInsn.h"
#include "Function.h"
#include "FunctionCallInsn.h"
#include "GoToInsn.h"
#include "ReturnInsn.h"

namespace nballerina {

TerminatorInsnCodeGen::TerminatorInsnCodeGen(FunctionCodeGen &functionGenerator, PackageCodeGen &)
    : functionGenerator(functionGenerator) {}

void TerminatorInsnCodeGen::visit(ConditionBrInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    auto *lhsTemp = functionGenerator.createTempVal(obj.lhsOp, module, builder);
    auto *brCondition = builder.CreateIsNotNull(lhsTemp, obj.lhsOp.getName());
    assert(functionGenerator.getBasicBlock(obj.getNextBBID()) != nullptr);
    assert(functionGenerator.getBasicBlock(obj.getElseBBID()) != nullptr);
    builder.CreateCondBr(brCondition, functionGenerator.getBasicBlock(obj.getNextBBID()),
                         functionGenerator.getBasicBlock(obj.getElseBBID()));
}

void TerminatorInsnCodeGen::visit(FunctionCallInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    std::vector<llvm::Value *> paramRefs;
    paramRefs.reserve(obj.argCount);
    for (const auto &arg : obj.argsList) {
        paramRefs.push_back(functionGenerator.createTempVal(arg, module, builder));
    }

    auto *lhsRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp, module);
    auto *namedFuncRef = module.getFunction(obj.functionName);
    auto *callResult = builder.CreateCall(namedFuncRef, paramRefs, "call");
    builder.CreateStore(callResult, lhsRef);

    // creating branch to next basic block.
    auto *nextBB = functionGenerator.getBasicBlock(obj.getNextBBID());
    if (nextBB != nullptr) {
        builder.CreateBr(nextBB);
    }
}

void TerminatorInsnCodeGen::visit(GoToInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) {
    assert(functionGenerator.getBasicBlock(obj.getNextBBID()) != nullptr);
    builder.CreateBr(functionGenerator.getBasicBlock(obj.getNextBBID()));
}

void TerminatorInsnCodeGen::visit(ReturnInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    if (funcObj.isMainFunction()) {
        builder.CreateRetVoid();
        return;
    }
    assert(funcObj.getReturnVar().has_value());
    auto *retValueRef =
        builder.CreateLoad(functionGenerator.getLocalVal(funcObj.getReturnVar()->getName()), "return_val_temp");
    builder.CreateRet(retValueRef);
}
} // namespace nballerina
