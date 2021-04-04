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

#include "GoToInsn.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Operand.h"
#include "llvm-c/Core.h"

namespace nballerina {

GoToInsn::GoToInsn(std::shared_ptr<BasicBlock> nextBB, std::shared_ptr<BasicBlock> currentBB)
    : TerminatorInsn(Operand("", NOT_A_KIND), std::move(currentBB), std::move(nextBB), true) {
    kind = INSTRUCTION_KIND_GOTO;
}

void GoToInsn::translate(LLVMModuleRef &) {
    LLVMBuilderRef builder = getFunctionRef().getLLVMBuilder();
    assert(getNextBB() != nullptr);
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
}

} // namespace nballerina
