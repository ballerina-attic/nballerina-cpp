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

#ifndef __FUNCTION__H__
#define __FUNCTION__H__

#include "bir/BasicBlock.h"
#include "bir/FunctionParam.h"
#include "bir/Operand.h"
#include "bir/RestParam.h"
#include "bir/Variable.h"
#include "interfaces/Debuggable.h"
#include <optional>
#include <string>
#include <vector>

namespace nballerina {

class Package;

class Function : public Debuggable {
  private:
    inline static const std::string MAIN_FUNCTION_NAME = "main";
    static constexpr unsigned int PUBLIC = 1;
    static constexpr unsigned int NATIVE = PUBLIC << 1;
    Package *parentPackage;
    std::string name;
    std::string workerName;
    unsigned int flags;
    std::optional<Variable> returnVar;
    std::optional<RestParam> restParam;
    std::vector<Variable> localVars;
    std::vector<BasicBlock> basicBlocks;
    std::vector<FunctionParam> requiredParams;

  public:
    Function(Package *parentPackage, std::string name, std::string workerName, unsigned int flags);
    Function(const Function &) = delete;
    Function(Function &&) noexcept = default;
    Function &operator=(const Function &) = delete;
    Function &operator=(Function &&) noexcept = default;

    const std::string &getName() const;
    size_t getNumParams() const;
    const std::optional<RestParam> &getRestParam() const;
    const std::optional<Variable> &getReturnVar() const;
    const Variable &getLocalVariable(const std::string &opName) const;
    const Variable &getLocalOrGlobalVariable(const Operand &op) const;
    bool isMainFunction() const;
    bool isExternalFunction() const;
    const std::vector<FunctionParam> &getParams() const;

    friend class FunctionCodeGen;
    friend class BIRReadFunction;
    friend class BIRReadBasicBlock;
};
} // namespace nballerina

#endif //!__FUNCTION__H__
