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

#include "NonTerminatorInsn.h"
#include <variant>

namespace nballerina {

enum MapConstrctBodyKind { Spread_Field_Kind = 0, Key_Value_Kind = 1 };

class MapConstruct {
  public:
    class SpreadField {
      private:
        Operand expr;

      public:
        SpreadField() = delete;
        SpreadField(const Operand &expr) : expr(expr) {}
        ~SpreadField() = default;

        const Operand &getExpr() const { return expr; }
    };
    class KeyValue {
      private:
        Operand keyOp;
        Operand valueOp;

      public:
        KeyValue() = delete;
        KeyValue(const Operand &key, const Operand &value) : keyOp(key), valueOp(value) {}
        ~KeyValue() = default;

        const Operand &getKey() const { return keyOp; }
        const Operand &getValue() const { return valueOp; }
    };

  private:
    MapConstrctBodyKind kind;
    std::variant<KeyValue, SpreadField> initValueStruct;

  public:
    MapConstruct() = delete;
    MapConstruct(KeyValue initVal) : kind(Key_Value_Kind), initValueStruct(initVal) {}
    MapConstruct(SpreadField initVal) : kind(Spread_Field_Kind), initValueStruct(initVal) {}
    virtual ~MapConstruct() = default;
    MapConstrctBodyKind getKind() const { return kind; }
    const std::variant<KeyValue, SpreadField> &getInitValStruct() const { return initValueStruct; }
};

class MapStoreInsn : public NonTerminatorInsn {
  private:
    Operand keyOp;
    Operand rhsOp;

  public:
    MapStoreInsn() = delete;
    MapStoreInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp, const Operand &ROp);
    ~MapStoreInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
    static void codeGenMapStore(LLVMBuilderRef builder, LLVMValueRef mapStoreFunc, LLVMValueRef map, LLVMValueRef key,
                                LLVMValueRef value);
};
} // namespace nballerina

#endif //!__MAPSTOREINSN__H__
