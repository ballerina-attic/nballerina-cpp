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

#include "FunctionCallInsn.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include <vector>

namespace nballerina {

FunctionCallInsn::FunctionCallInsn(BasicBlock &currentBB, std::string thenBBID, const Operand &lhs,
                                   std::string functionName, int argCount, std::vector<Operand> argsList)
    : TerminatorInsn(lhs, currentBB, std::move(thenBBID), true), functionName(std::move(functionName)),
      argCount(argCount), argsList(std::move(argsList)) {
    kind = INSTRUCTION_KIND_CALL;
}

void FunctionCallInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    std::vector<llvm::Value *> paramRefs;
    paramRefs.reserve(argCount);
    for (const auto &arg : argsList) {
        paramRefs.push_back(funcObj.createTempVariable(arg, module, builder));
    }

    auto *lhsRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *namedFuncRef = module.getFunction(functionName);
    auto *callResult = builder.CreateCall(namedFuncRef, paramRefs, "call");
    builder.CreateStore(callResult, lhsRef);

    // creating branch to next basic block.
    if (getNextBB().getLLVMBBRef() != nullptr) {
        builder.CreateBr(getNextBB().getLLVMBBRef());
    }
}

} // namespace nballerina
