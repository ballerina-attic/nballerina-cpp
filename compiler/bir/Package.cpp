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

#include "bir/Package.h"
#include <algorithm>
#include <cassert>

namespace nballerina {

std::string Package::getModuleName() const { return org + name + version; }

const Function &Package::getFunction(const std::string &name) const {
    auto result = std::find_if(functions.begin(), functions.end(),
                               [&name](const Function &i) -> bool { return i.getName() == name; });
    assert(result != functions.end());
    return *result;
}

const Variable &Package::getGlobalVariable(const std::string &name) const {
    auto result = std::find_if(globalVars.begin(), globalVars.end(),
                               [&name](const Variable &i) -> bool { return i.getName() == name; });
    assert(result != globalVars.end());
    return *result;
}

} // namespace nballerina
