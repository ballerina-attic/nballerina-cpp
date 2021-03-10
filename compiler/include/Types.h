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

#ifndef __TYPEDECL__H__
#define __TYPEDECL__H__

#include <optional>
#include <string>

namespace nballerina {

enum TypeTag {
    TYPE_TAG_INVALID = 0,
    TYPE_TAG_INT = 1,
    TYPE_TAG_BYTE = 2,
    TYPE_TAG_FLOAT = 3,
    TYPE_TAG_DECIMAL = 4,
    TYPE_TAG_STRING = 5,
    TYPE_TAG_BOOLEAN = 6,
    TYPE_TAG_JSON = 7,
    TYPE_TAG_XML = 8,
    TYPE_TAG_TABLE = 9,
    TYPE_TAG_NIL = 10,
    TYPE_TAG_ANYDATA = 11,
    TYPE_TAG_RECORD = 12,
    TYPE_TAG_TYPEDESC = 13,
    TYPE_TAG_STREAM = 14,
    TYPE_TAG_MAP = 15,
    TYPE_TAG_INVOKABLE = 16,
    TYPE_TAG_ANY = 17,
    TYPE_TAG_ENDPOINT = 18,
    TYPE_TAG_ARRAY = 19,
    TYPE_TAG_UNION = 20,
    TYPE_TAG_INTERSECTION = 21,
    TYPE_TAG_PACKAGE = 22,
    TYPE_TAG_NONE = 23,
    TYPE_TAG_VOID = 24,
    TYPE_TAG_XMLNS = 25,
    TYPE_TAG_ANNOTATION = 26,
    TYPE_TAG_SEMANTIC_ERROR = 27,
    TYPE_TAG_ERROR = 28,
    TYPE_TAG_ITERATOR = 29,
    TYPE_TAG_TUPLE = 30,
    TYPE_TAG_FUTURE = 31,
    TYPE_TAG_FINITE = 32,
    TYPE_TAG_OBJECT = 33,
    TYPE_TAG_SERVICE = 34,
    TYPE_TAG_BYTE_ARRAY = 35,
    TYPE_TAG_FUNCTION_POINTER = 36,
    TYPE_TAG_HANDLE = 37,
    TYPE_TAG_READONLY = 38,
    TYPE_TAG_SIGNED32_INT = 39,
    TYPE_TAG_SIGNED16_INT = 40,
    TYPE_TAG_SIGNED8_INT = 41,
    TYPE_TAG_UNSIGNED32_INT = 42,
    TYPE_TAG_UNSIGNED16_INT = 43,
    TYPE_TAG_UNSIGNED8_INT = 44,
    TYPE_TAG_CHAR_STRING = 45,
    TYPE_TAG_XML_ELEMENT = 46,
    TYPE_TAG_XML_PI = 47,
    TYPE_TAG_XML_COMMENT = 48,
    TYPE_TAG_XML_TEXT = 49,
    TYPE_TAG_NEVER = 50,
    TYPE_TAG_NULL_SET = 51,
    TYPE_TAG_PARAMETERIZED_TYPE = 52
};

class Type {

  private:
    TypeTag type;
    std::string name;
    int flags;
    std::optional<TypeTag> memberType;

  public:
    Type() = delete;
    Type(TypeTag type, std::string name, int flags);
    Type(TypeTag type, std::string name, int flags, TypeTag memberType);
    virtual ~Type() = default;

    TypeTag getTypeTag() const;
    const std::string &getName() const;
    static std::string getNameOfType(TypeTag typeTag);
    TypeTag getMemberTypeTag() const;
};

} // namespace nballerina

#endif //!__TYPEDECL__H__
