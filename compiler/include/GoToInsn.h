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

#ifndef __GOTOINSN__H__
#define __GOTOINSN__H__

#include "TerminatorInsn.h"

namespace nballerina {

class GoToInsn : public TerminatorInsn, public Translatable<GoToInsn> {
  public:
    GoToInsn() = delete;
    GoToInsn(BasicBlock &currentBB, std::string thenBBID)
        : TerminatorInsn(Operand("", NOT_A_KIND), currentBB, std::move(thenBBID)) {
        kind = INSTRUCTION_KIND_GOTO;
    }
    ~GoToInsn() = default;

    friend class TerminatorInsnCodeGen;
};

} // namespace nballerina

#endif //!__GOTOINSN__H__
