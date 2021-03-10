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

#include "Types.h"

namespace nballerina {

Type::Type(TypeTag type, std::string namep, int flagsp, TypeTag memberType)
    : type(type), name(std::move(namep)), flags(flagsp), memberType(memberType) {}

Type::Type(TypeTag type, std::string namep, int flagsp)
    : type(type), name(std::move(namep)), flags(flagsp) {}

TypeTag Type::getTypeTag() const { return type; }
const std::string &Type::getName() const { return name; }

TypeTag Type::getMemberTypeTag() const {
    if (memberType) {
        return memberType.value();
    }
    return TYPE_TAG_INVALID;
}

std::string Type::getNameOfType(TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT:
        return "int";
    case TYPE_TAG_FLOAT:
        return "float";
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING:
        return "string";
    case TYPE_TAG_BOOLEAN:
        return "bool";
    case TYPE_TAG_ANY:
        return "any";
    default:
        return "";
    }
}

} // namespace nballerina
