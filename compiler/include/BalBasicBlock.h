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

#ifndef __BALBASICBLOCK__H__
#define __BALBASICBLOCK__H__

#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include "interfaces/Translatable.h"
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class Function;
class TerminatorInsn;
class NonTerminatorInsn;

class BasicBlock : public PackageNode, public Debuggable, public Translatable {
  private:
    std::string id;
    Function *parentFunction;
    TerminatorInsn *terminator;
    BasicBlock *nextBB;
    std::vector<NonTerminatorInsn *> instructions;
    LLVMBasicBlockRef bbRefObj;

  public:
    BasicBlock() = delete;
    BasicBlock(std::string id, Function *parentFunc);
    ~BasicBlock() = default;

    std::string &getId();
    TerminatorInsn *getTerminatorInsn();
    Function *getFunction();
    BasicBlock *getNextBB();
    LLVMBasicBlockRef getLLVMBBRef();
    Package *getPackage() final;

    void setNextBB(BasicBlock *bb);
    void setTerminatorInsn(TerminatorInsn *insn);
    void addNonTermInsn(NonTerminatorInsn *insn);
    void setLLVMBBRef(LLVMBasicBlockRef bbRef);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALBASICBLOCK__H__
