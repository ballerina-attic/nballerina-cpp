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

#include "BalTerminatorInsn.h"

namespace nballerina {

TerminatorInsn::TerminatorInsn(Operand *lOp, BasicBlock *currentBB, BasicBlock *then, bool _patchRequire)
    : Instruction(lOp, currentBB), thenBB(then), patchRequire(_patchRequire), kind() {}

BasicBlock *TerminatorInsn::getNextBB() { return thenBB; }
bool TerminatorInsn::getPatchStatus() { return patchRequire; }
InstructionKind TerminatorInsn::getInstKind() { return kind; }
void TerminatorInsn::setPatched() { patchRequire = false; }
void TerminatorInsn::setNextBB(BasicBlock *bb) { thenBB = bb; }

void TerminatorInsn::translate([[maybe_unused]] LLVMModuleRef &modRef) {}

} // namespace nballerina
