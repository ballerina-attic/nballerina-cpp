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

#ifndef __BALTERMINATORINSN__H__
#define __BALTERMINATORINSN__H__

#include "interfaces/Instruction.h"
#include "interfaces/Translatable.h"

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;

class TerminatorInsn : public Instruction, public Translatable {
  private:
    BasicBlock *thenBB;
    bool patchRequire = false;

  protected:
    InstructionKind kind;

  public:
    TerminatorInsn() = delete;
    TerminatorInsn(Operand *lOp, BasicBlock *currentBB, BasicBlock *then, bool _patchRequire);
    virtual ~TerminatorInsn() = default;

    BasicBlock *getNextBB();
    bool getPatchStatus();
    InstructionKind getInstKind();
    void setPatched();
    void setNextBB(BasicBlock *bb);

    virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALTERMINATORINSN__H__
