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

#ifndef __TERMINATORINSN__H__
#define __TERMINATORINSN__H__

#include "interfaces/AbstractInstruction.h"
#include "interfaces/Translatable.h"

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;

class TerminatorInsn : public AbstractInstruction, public Translatable {
  private:
    std::shared_ptr<BasicBlock> thenBB;
    bool patchRequired = false;

  protected:
    InstructionKind kind;

  public:
    TerminatorInsn() = delete;
    TerminatorInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, std::shared_ptr<BasicBlock> then,
                   bool patchRequired = false);
    virtual ~TerminatorInsn() = default;

    BasicBlock *getNextBB() const;
    bool isPatched() const;
    InstructionKind getInstKind() const;

    void setPatched();
    void setNextBB(std::shared_ptr<BasicBlock> bb);

    virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__TERMINATORINSN__H__
