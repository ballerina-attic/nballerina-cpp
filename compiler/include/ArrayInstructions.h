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

#ifndef __ARRAYINSNS__H__
#define __ARRAYINSNS__H__

#include "NonTerminatorInsn.h"
#include "Types.h"

namespace nballerina {

class ArrayInsn : public NonTerminatorInsn, public Translatable<ArrayInsn> {
  private:
    Operand sizeOp;

  public:
    ArrayInsn() = delete;
    ArrayInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &sizeOp)
        : NonTerminatorInsn(lhs, currentBB), sizeOp(sizeOp) {}
    ~ArrayInsn() = default;
    friend class NonTerminatorInsnCodeGen;
};

class ArrayLoadInsn : public NonTerminatorInsn, public Translatable<ArrayLoadInsn> {
  private:
    Operand keyOp;
    Operand rhsOp;

  public:
    ArrayLoadInsn() = delete;
    ArrayLoadInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &KOp, const Operand &ROp)
        : NonTerminatorInsn(lhs, currentBB), keyOp(KOp), rhsOp(ROp) {}
    ~ArrayLoadInsn() = default;
    friend class NonTerminatorInsnCodeGen;
};

class ArrayStoreInsn : public NonTerminatorInsn, public Translatable<ArrayStoreInsn> {
  private:
    Operand keyOp;
    Operand rhsOp;

  public:
    ArrayStoreInsn() = delete;
    ArrayStoreInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &KOp, const Operand &ROp)
        : NonTerminatorInsn(lhs, currentBB), keyOp(KOp), rhsOp(ROp) {}
    ~ArrayStoreInsn() = default;
    friend class NonTerminatorInsnCodeGen;
};

} // namespace nballerina

#endif //!__ARRAYINSNS__H__
