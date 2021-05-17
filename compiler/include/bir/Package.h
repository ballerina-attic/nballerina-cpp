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

#ifndef __PACKAGE__H__
#define __PACKAGE__H__

#include "bir/Function.h"
#include "bir/Variable.h"
#include <string>
#include <vector>

namespace nballerina {

class Package {

  private:
    std::string org;
    std::string name;
    std::string version;
    std::string sourceFileName;
    std::vector<Variable> globalVars;
    std::vector<Function> functions;

  public:
    Package() = default;
    Package(const Package &) = delete;
    Package(Package &&) noexcept = delete;
    Package &operator=(const Package &) = delete;
    Package &operator=(Package &&) noexcept = delete;

    std::string getModuleName() const;
    const Function &getFunction(const std::string &name) const;
    const Variable &getGlobalVariable(const std::string &name) const;

    friend class PackageCodeGen;
    friend class BIRReadPackage;
    friend class BIRReadFunction;
};

} // namespace nballerina

#endif //!__PACKAGE__H__
