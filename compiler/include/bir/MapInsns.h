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

#ifndef __MAPSTOREINSN__H__
#define __MAPSTOREINSN__H__

#include "interfaces/NonTerminatorInsn.h"
#include <variant>

namespace nballerina {

enum MapConstrctBodyKind { Spread_Field_Kind = 0, Key_Value_Kind = 1 };

class MapConstruct {
  public:
    class SpreadField {
      private:
        Operand expr;

      public:
        SpreadField(Operand expr) : expr(std::move(expr)) {}
        const Operand &getExpr() const { return expr; }
    };
    class KeyValue {
      private:
        Operand keyOp;
        Operand valueOp;

      public:
        KeyValue(Operand key, Operand value) : keyOp(std::move(key)), valueOp(std::move(value)) {}
        const Operand &getKey() const { return keyOp; }
        const Operand &getValue() const { return valueOp; }
    };

  private:
    MapConstrctBodyKind kind;
    std::variant<KeyValue, SpreadField> initValueStruct;

  public:
    MapConstruct(KeyValue initVal) : kind(Key_Value_Kind), initValueStruct(std::move(initVal)) {}
    MapConstruct(SpreadField initVal) : kind(Spread_Field_Kind), initValueStruct(std::move(initVal)) {}
    MapConstrctBodyKind getKind() const { return kind; }
    const std::variant<KeyValue, SpreadField> &getInitValStruct() const { return initValueStruct; }
};

class MapStoreInsn : public NonTerminatorInsn, public Translatable<MapStoreInsn> {
  private:
    Operand keyOp;
    Operand rhsOp;

  public:
    MapStoreInsn(Operand lhs, BasicBlock &currentBB, Operand KOp, Operand ROp)
        : NonTerminatorInsn(std::move(lhs), currentBB), keyOp(std::move(KOp)), rhsOp(std::move(ROp)) {}
    friend class NonTerminatorInsnCodeGen;
};

class MapLoadInsn : public NonTerminatorInsn, public Translatable<MapLoadInsn> {
  private:
    Operand keyOp;
    Operand rhsOp;

  public:
    MapLoadInsn(Operand lhs, BasicBlock &currentBB, Operand KOp, Operand ROp)
        : NonTerminatorInsn(std::move(lhs), currentBB), keyOp(std::move(KOp)), rhsOp(std::move(ROp)) {}
    friend class NonTerminatorInsnCodeGen;
};
} // namespace nballerina

#endif //!__MAPSTOREINSN__H__
