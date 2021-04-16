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
#include <cassert>

namespace nballerina {

Type::Type(TypeTag type, std::string namep) : type(type), name(std::move(namep)) {}

Type::Type(TypeTag type, std::string namep, ArrayType arrayType)
    : type(type), name(std::move(namep)), typeInfo(arrayType) {}

Type::Type(TypeTag type, std::string namep, MapType mapType) : type(type), name(std::move(namep)), typeInfo(mapType) {}

TypeTag Type::getTypeTag() const { return type; }
const std::string &Type::getName() const { return name; }

TypeTag Type::getMemberTypeTag() const {
    if (type == TYPE_TAG_ARRAY) {
        return std::get<Type::ArrayType>(typeInfo).memberType;
    }
    if (type == TYPE_TAG_MAP) {
        return std::get<Type::MapType>(typeInfo).memberType;
    }
    return TYPE_TAG_INVALID;
}

std::string Type::getNameOfType(TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT:
        return "int";
    case TYPE_TAG_FLOAT:
        return "float";
    case TYPE_TAG_STRING:
        return "string";
    case TYPE_TAG_BOOLEAN:
        return "bool";
    case TYPE_TAG_ANY:
        return "any";
    case TYPE_TAG_UNION:
        return "union";
    default:
        assert(false);
        return "";
    }
}

std::string_view Type::typeStringMangleName(const Type &type) {
    switch (type.getTypeTag()) {
    case TYPE_TAG_INT: {
        return "__I";
    }
    case TYPE_TAG_FLOAT: {
        return "__F";
    }
    case TYPE_TAG_STRING: {
        return "__S";
    }
    case TYPE_TAG_BOOLEAN: {
        return "__B";
    }
    case TYPE_TAG_ARRAY: {
        // TODO add array type and size
        return "__A";
    }
    case TYPE_TAG_ANY: {
        return "__X";
    }
    case TYPE_TAG_NIL: {
        return "__N";
    }
    case TYPE_TAG_MAP: {
        TypeTag memberTypeTag = type.getMemberTypeTag();
        switch (memberTypeTag) {
        case TYPE_TAG_INT:
            return "__MI";
        default:
            return "__M";
        }
    }
    default:
        return "";
    }
}

} // namespace nballerina
