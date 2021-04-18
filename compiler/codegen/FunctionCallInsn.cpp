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
#include "llvm-c/Core.h"

using namespace std;

namespace nballerina {

FunctionCallInsn::FunctionCallInsn(std::weak_ptr<BasicBlock> currentBB, std::string thenBBID, const Operand &lhs,
                                   std::string functionName, int argCount, std::vector<Operand> argsList)
    : TerminatorInsn(lhs, std::move(currentBB), std::move(thenBBID), true), functionName(std::move(functionName)),
      argCount(argCount), argsList(std::move(argsList)) {
    kind = INSTRUCTION_KIND_CALL;
}

void FunctionCallInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    std::unique_ptr<LLVMValueRef[]> ParamRefs(new LLVMValueRef[argCount]);

    for (int i = 0; i < argCount; i++) {
        auto op = argsList[i];
        LLVMValueRef opRef = llvm::wrap(funcObj.createTempVariable(op, module, builder));
        ParamRefs[i] = opRef;
    }

    LLVMValueRef lhsRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMValueRef namedFuncRef = llvm::wrap(module.getFunction(functionName));
    LLVMValueRef callResult = LLVMBuildCall(llvm::wrap(&builder), namedFuncRef, ParamRefs.get(), argCount, "call");
    LLVMBuildStore(llvm::wrap(&builder), callResult, lhsRef);

    // creating branch to next basic block.
    if (getNextBB().getLLVMBBRef() != nullptr) {
        LLVMBuildBr(llvm::wrap(&builder), llvm::wrap(getNextBB().getLLVMBBRef()));
    }
}

} // namespace nballerina
