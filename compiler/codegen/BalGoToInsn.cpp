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

#include "BalGoToInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

namespace nballerina {

GoToInsn::GoToInsn(BasicBlock *nextBB, BasicBlock *currentBB) : TerminatorInsn(nullptr, currentBB, nextBB, true) {
    kind = INSTRUCTION_KIND_GOTO;
}

void GoToInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {
    LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
}

} // namespace nballerina
