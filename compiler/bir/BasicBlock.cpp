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

#include "bir/BasicBlock.h"
#include "bir/Function.h"

namespace nballerina {

BasicBlock::BasicBlock(std::string pid, Function *parentFunc)
    : id(std::move(pid)), parentFunction(parentFunc), terminator(nullptr) {}

const std::string &BasicBlock::getId() const { return id; }
TerminatorInsn *BasicBlock::getTerminatorInsnPtr() const { return terminator.get(); }

const Function &BasicBlock::getParentFunctionRef() const { return *parentFunction; }

void BasicBlock::setTerminatorInsn(std::unique_ptr<TerminatorInsn> insn) { terminator = std::move(insn); }
void BasicBlock::addNonTermInsn(std::unique_ptr<NonTerminatorInsn> insn) { instructions.push_back(std::move(insn)); }

} // namespace nballerina
