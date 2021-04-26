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

#include "Package.h"
#include <cassert>

namespace nballerina {

std::string Package::getModuleName() const { return org + name + version; }

void Package::setOrgName(std::string orgName) { org = std::move(orgName); }

void Package::setPackageName(std::string pkgName) { name = std::move(pkgName); }

void Package::setVersion(std::string verName) { version = std::move(verName); }

void Package::setSrcFileName(std::string srcFileName) { sourceFileName = std::move(srcFileName); }

void Package::insertFunction(const std::shared_ptr<Function> &function) {
    functionLookUp.insert(std::pair<std::string, std::shared_ptr<Function>>(function->getName(), function));
}

const Function &Package::getFunction(const std::string &name) const { return *functionLookUp.at(name); }

const Variable &Package::getGlobalVariable(const std::string &name) const {
    const auto &varIt = globalVars.find(name);
    assert(varIt != globalVars.end());
    return varIt->second;
}

void Package::insertGlobalVar(const Variable &var) {
    globalVars.insert(std::pair<std::string, Variable>(var.getName(), var));
}

} // namespace nballerina
