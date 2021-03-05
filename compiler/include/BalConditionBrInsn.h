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

#ifndef __BALCONDITIONBRINSN__H__
#define __BALCONDITIONBRINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class ConditionBrInsn : public TerminatorInsn {
  private:
    BasicBlock *ifThenBB;
    BasicBlock *elseBB;

  public:
    ConditionBrInsn() = delete;
    ConditionBrInsn(Operand *lOp, BasicBlock *currentBB, BasicBlock *_ifThenBB, BasicBlock *_elseBB);
    ~ConditionBrInsn() = default;

    BasicBlock *getIfThenBB();
    BasicBlock *getElseBB();

    void setIfThenBB(BasicBlock *bb);
    void setElseBB(BasicBlock *bb);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONDITIONBRINSN__H__
