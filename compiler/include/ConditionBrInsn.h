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

#ifndef __CONDITIONBRINSN__H__
#define __CONDITIONBRINSN__H__

#include "TerminatorInsn.h"
#include <memory>

namespace nballerina {

class ConditionBrInsn : public TerminatorInsn, public Translatable<ConditionBrInsn> {
  private:
    std::string elseBBID;

  public:
    ConditionBrInsn() = delete;
    ConditionBrInsn(const Operand &lhs, BasicBlock &currentBB, std::string ifBBID, std::string elseBBID)
        : TerminatorInsn(lhs, currentBB, ifBBID), elseBBID(std::move(elseBBID)) {
        kind = INSTRUCTION_KIND_CONDITIONAL_BRANCH;
    }
    ~ConditionBrInsn() = default;

    const std::string &getElseBBID() const { return elseBBID; }
    friend class TerminatorInsnCodeGen;
};

} // namespace nballerina

#endif //!__CONDITIONBRINSN__H__
