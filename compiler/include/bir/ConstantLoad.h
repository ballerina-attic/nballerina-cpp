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
    std::variant<int64_t, double, bool, uint8_t, std::string> value;
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, int64_t intVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_INT), value(intVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, uint8_t byteVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_BYTE), value(byteVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, double doubleVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_FLOAT), value(doubleVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, bool boolVal)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_BOOLEAN), value(boolVal) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB, std::string str)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_STRING), value(std::move(str)) {}
    ConstantLoadInsn(Operand lhs, BasicBlock &currentBB)
        : NonTerminatorInsn(std::move(lhs), currentBB), typeTag(TYPE_TAG_NIL) {}

  public:
    static std::unique_ptr<ConstantLoadInsn> createIntConstLoad(Operand lhs, BasicBlock &currentBB, int64_t intVal) {
        return std::unique_ptr<ConstantLoadInsn>(new ConstantLoadInsn(std::move(lhs), currentBB, intVal));
    }

    static std::unique_ptr<ConstantLoadInsn> createByteConstLoad(Operand lhs, BasicBlock &currentBB, uint8_t byteVal) {
        return std::unique_ptr<ConstantLoadInsn>(new ConstantLoadInsn(std::move(lhs), currentBB, byteVal));
    }

    static std::unique_ptr<ConstantLoadInsn> createFloatConstLoad(Operand lhs, BasicBlock &currentBB,
                                                                  double doubleVal) {
        return std::unique_ptr<ConstantLoadInsn>(new ConstantLoadInsn(std::move(lhs), currentBB, doubleVal));
    }

    static std::unique_ptr<ConstantLoadInsn> createBoolConstLoad(Operand lhs, BasicBlock &currentBB, bool boolVal) {
        return std::unique_ptr<ConstantLoadInsn>(new ConstantLoadInsn(std::move(lhs), currentBB, boolVal));
    }

    static std::unique_ptr<ConstantLoadInsn> createStringConstLoad(Operand lhs, BasicBlock &currentBB,
                                                                   std::string str) {
        return std::unique_ptr<ConstantLoadInsn>(new ConstantLoadInsn(std::move(lhs), currentBB, str));
    }

    static std::unique_ptr<ConstantLoadInsn> createNullConstLoad(Operand lhs, BasicBlock &currentBB) {
        return std::unique_ptr<ConstantLoadInsn>(new ConstantLoadInsn(std::move(lhs), currentBB));
    }
    friend class NonTerminatorInsnCodeGen;
};

} // namespace nballerina

#endif //!__CONSTANTLOAD__H__
