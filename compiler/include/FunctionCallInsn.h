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

#ifndef __FUNCTIONCALLINSN__H__
#define __FUNCTIONCALLINSN__H__

#include "RestParam.h"
#include "TerminatorInsn.h"
#include <string>
#include <vector>

namespace nballerina {

class FunctionCallInsn : public TerminatorInsn {
  private:
    std::string functionName;
    int argCount;
    std::vector<Operand> argsList;

  public:
    FunctionCallInsn() = delete;
    FunctionCallInsn(std::string _funcName, int argNumber, std::shared_ptr<BasicBlock> nextBB, const Operand &lhs,
                     std::vector<Operand> fnArgs, std::shared_ptr<BasicBlock> currentBB);
    ~FunctionCallInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__FUNCTIONCALLINSN__H__
