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

#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "BalNonTerminatorInsn.h"
#include "BalTerminatorInsn.h"

namespace nballerina {

BasicBlock::BasicBlock(std::string pid, Function *parentFunc) : id(pid), parentFunction(parentFunc) {}

std::string &BasicBlock::getId() { return id; }
TerminatorInsn *BasicBlock::getTerminatorInsn() { return terminator; }
Function *BasicBlock::getFunction() { return parentFunction; }
BasicBlock *BasicBlock::getNextBB() { return nextBB; }
LLVMBasicBlockRef BasicBlock::getLLVMBBRef() { return bbRefObj; }
Package *BasicBlock::getPackage() { return parentFunction->getPackage(); }

void BasicBlock::setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
void BasicBlock::setNextBB(BasicBlock *bb) { nextBB = bb; }
void BasicBlock::setLLVMBBRef(LLVMBasicBlockRef bbRef) { bbRefObj = bbRef; }
void BasicBlock::addNonTermInsn(NonTerminatorInsn *insn) { instructions.push_back(insn); }

void BasicBlock::translate(LLVMModuleRef &modRef) {
    for (const auto &instruction : instructions) {
        instruction->translate(modRef);
    }
    if (terminator) {
        terminator->translate(modRef);
    }
}

} // namespace nballerina
