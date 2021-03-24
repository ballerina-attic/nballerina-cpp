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

#include "BasicBlock.h"
#include "Function.h"
#include "NonTerminatorInsn.h"
#include "TerminatorInsn.h"

namespace nballerina {

BasicBlock::BasicBlock(std::string pid, std::shared_ptr<Function> parentFunc)
    : id(std::move(pid)), parentFunction(std::move(parentFunc)), terminator(nullptr), nextBB(nullptr),
      bbRefObj(nullptr) {}

const std::string &BasicBlock::getId() const { return id; }
TerminatorInsn *BasicBlock::getTerminatorInsnPtr() const { return terminator.get(); }
std::shared_ptr<Function> BasicBlock::getFunctionSharedObj() const { return parentFunction; }
const Function &BasicBlock::getParentFunctionRef() const { return *parentFunction; }
LLVMBasicBlockRef BasicBlock::getLLVMBBRef() const { return bbRefObj; }

void BasicBlock::setTerminatorInsn(std::unique_ptr<TerminatorInsn> insn) { terminator = std::move(insn); }
void BasicBlock::setNextBB(std::shared_ptr<BasicBlock> bb) { nextBB = std::move(bb); }
void BasicBlock::setLLVMBBRef(LLVMBasicBlockRef bbRef) { bbRefObj = bbRef; }
void BasicBlock::addNonTermInsn(std::unique_ptr<NonTerminatorInsn> insn) { instructions.push_back(std::move(insn)); }

void BasicBlock::translate(LLVMModuleRef &modRef) {
    for (const auto &instruction : instructions) {
        instruction->translate(modRef);
    }
    if (terminator != nullptr) {
        terminator->translate(modRef);
    }
}

} // namespace nballerina
