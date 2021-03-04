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

#ifndef __BALFUNCTIONCALLINSN__H__
#define __BALFUNCTIONCALLINSN__H__

#include "BalRestParam.h"
#include "BalTerminatorInsn.h"
#include <string>
#include <vector>

namespace nballerina {

class FunctionCallInsn : public TerminatorInsn {
  private:
    bool isVirtual;
    std::string functionName;
    int argCount;
    std::vector<Operand *> argsList;
    RestParam *restParam;

  public:
    FunctionCallInsn() = delete;
    FunctionCallInsn(bool funcVirtual, std::string _funcName, int argNumber, BasicBlock *nextBB, Operand *lhsOp,
                     std::vector<Operand *> &fnArgs, BasicBlock *currentBB);
    ~FunctionCallInsn() = default;

    std::string getFunctionName();
    int getArgCount();
    Operand *getArgumentFromList(int i);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALFUNCTIONCALLINSN__H__
