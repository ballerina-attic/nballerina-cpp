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
#include <map>
#include <memory>
#include <string>

namespace nballerina {

class Package {

  private:
    std::string org;
    std::string name;
    std::string version;
    std::string sourceFileName;
    std::map<std::string, Variable> globalVars;
    std::map<std::string, std::shared_ptr<Function>> functionLookUp;

  public:
    Package() = default;
    virtual ~Package() = default;
    Package(const Package &) = delete;
    Package(Package &&obj) noexcept = delete;
    Package &operator=(const Package &obj) = delete;
    Package &operator=(Package &&obj) noexcept = delete;

    std::string getModuleName() const;
    const Function &getFunction(const std::string &name) const;
    const Variable &getGlobalVariable(const std::string &name) const;

    void setOrgName(std::string orgName);
    void setPackageName(std::string pkgName);
    void setVersion(std::string verName);
    void setSrcFileName(std::string srcFileName);
    void insertGlobalVar(const Variable &var);
    void insertFunction(const std::shared_ptr<Function> &function);

    friend class PackageCodeGen;
};

} // namespace nballerina

#endif //!__PACKAGE__H__
