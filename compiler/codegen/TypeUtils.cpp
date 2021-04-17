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

#include "TypeUtils.h"
#include "llvm/Support/ErrorHandling.h"

namespace nballerina {

void TypeUtils::checkMapSupport(TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT:
    case TYPE_TAG_ANYDATA:
        return;
    default:
        std::string msg = "Map of " + Type::getNameOfType(typeTag) + " is not currently supported";
        llvm_unreachable(msg.c_str());
    }
}

} // namespace nballerina
