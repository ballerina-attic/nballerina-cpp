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

#ifndef __CONSTANTLOAD__H__
#define __CONSTANTLOAD__H__

#include "bir/Types.h"
#include "interfaces/NonTerminatorInsn.h"
#include <string>
#include <variant>

namespace nballerina {

class ConstantLoadInsn : public NonTerminatorInsn, public Translatable<ConstantLoadInsn> {
  private:
    TypeTag typeTag;
    std::variant<int64_t, double, bool, std::string> value;

  public:
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, int64_t intVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_INT), value(intVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, double doubleVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_FLOAT), value(doubleVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, bool boolVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_BOOLEAN), value(boolVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, std::string str)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_STRING), value(std::move(str)) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_NIL) {}
    friend class NonTerminatorInsnCodeGen;
};

} // namespace nballerina

#endif //!__CONSTANTLOAD__H__
