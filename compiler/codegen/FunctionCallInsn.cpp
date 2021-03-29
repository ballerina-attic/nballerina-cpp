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

FunctionCallInsn::FunctionCallInsn(std::string funcName, int argNumber, std::shared_ptr<BasicBlock> nextBB,
                                   const Operand &lhs, std::vector<Operand> fnArgs,
                                   std::shared_ptr<BasicBlock> currentBB)
    : TerminatorInsn(lhs, std::move(currentBB), std::move(nextBB), true), functionName(std::move(funcName)), argCount(argNumber),
      argsList(std::move(fnArgs)) {
    kind = INSTRUCTION_KIND_CALL;
}

void FunctionCallInsn::translate(LLVMModuleRef &) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    std::unique_ptr<LLVMValueRef[]> ParamRefs(new LLVMValueRef[argCount]);

    const auto &function = getPackageRef().getFunction(functionName);
    for (int i = 0; i < argCount; i++) {
        auto op = argsList[i];
        LLVMValueRef opRef = funcObj.createTempVariable(op);
        ParamRefs[i] = opRef;
    }

    LLVMValueRef lhsRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef namedFuncRef = function.getLLVMFunctionValue();
    LLVMValueRef callResult = LLVMBuildCall(builder, namedFuncRef, ParamRefs.get(), argCount, "call");
    LLVMBuildStore(builder, callResult, lhsRef);

    // creating branch to next basic block.
    if (getNextBB() != nullptr) {
        LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
    }
}

} // namespace nballerina
