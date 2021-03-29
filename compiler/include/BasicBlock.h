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

#ifndef __BASICBLOCK__H__
#define __BASICBLOCK__H__

#include "interfaces/Debuggable.h"
#include "interfaces/Translatable.h"
#include <memory>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class Function;
class TerminatorInsn;
class NonTerminatorInsn;

class BasicBlock : public Debuggable, public Translatable {
  private:
    std::string id;
    std::shared_ptr<Function> parentFunction;
    std::unique_ptr<TerminatorInsn> terminator;
    std::shared_ptr<BasicBlock> nextBB;
    std::vector<std::unique_ptr<NonTerminatorInsn>> instructions;
    LLVMBasicBlockRef bbRefObj;

  public:
    BasicBlock() = delete;
    BasicBlock(std::string id, std::shared_ptr<Function> parentFunc);
    BasicBlock(const BasicBlock &) = delete;
    BasicBlock(BasicBlock &&obj) noexcept = delete;
    BasicBlock &operator=(const BasicBlock &obj) = delete;
    BasicBlock &operator=(BasicBlock &&obj) noexcept = delete;
    ~BasicBlock() = default;

    const std::string &getId() const;
    TerminatorInsn *getTerminatorInsnPtr() const;
    std::shared_ptr<Function> getFunctionSharedObj() const;
    const Function &getParentFunctionRef() const;
    LLVMBasicBlockRef getLLVMBBRef() const;

    void setNextBB(std::shared_ptr<BasicBlock> bb);
    void setTerminatorInsn(std::unique_ptr<TerminatorInsn> insn);
    void addNonTermInsn(std::unique_ptr<NonTerminatorInsn> insn);
    void setLLVMBBRef(LLVMBasicBlockRef bbRef);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BASICBLOCK__H__
