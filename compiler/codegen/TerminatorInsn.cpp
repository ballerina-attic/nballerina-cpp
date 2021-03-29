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

#include "TerminatorInsn.h"
#include <memory>

namespace nballerina {

TerminatorInsn::TerminatorInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB,
                               std::shared_ptr<BasicBlock> then, bool patchRequired)
    : AbstractInstruction(lhs, std::move(currentBB)), thenBB(std::move(then)), patchRequired(patchRequired),
      kind(INSTRUCTION_NOT_AN_INSTRUCTION) {}

BasicBlock *TerminatorInsn::getNextBB() const { return thenBB.get(); }
bool TerminatorInsn::isPatched() const { return patchRequired; }
InstructionKind TerminatorInsn::getInstKind() const { return kind; }
void TerminatorInsn::setPatched() { patchRequired = false; }
void TerminatorInsn::setNextBB(std::shared_ptr<BasicBlock> bb) { thenBB = std::move(bb); }

void TerminatorInsn::translate(LLVMModuleRef &) {}

} // namespace nballerina
