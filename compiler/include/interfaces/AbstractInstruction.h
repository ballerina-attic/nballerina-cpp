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

#ifndef __ABSTRACTINSN__H__
#define __ABSTRACTINSN__H__

#include "Operand.h"
#include "interfaces/Debuggable.h"
#include <memory>

namespace nballerina {

// Forward Declaration
class Function;
class BasicBlock;
class Package;

enum InstructionKind {
    INSTRUCTION_NOT_AN_INSTRUCTION = 0,
    INSTRUCTION_KIND_GOTO = 1,
    INSTRUCTION_KIND_CALL = 2,
    INSTRUCTION_KIND_CONDITIONAL_BRANCH = 3,
    INSTRUCTION_KIND_RETURN = 4,
    INSTRUCTION_KIND_BINARY_REF_EQUAL,
    INSTRUCTION_KIND_BINARY_REF_NOT_EQUAL,
    INSTRUCTION_KIND_BINARY_CLOSED_RANGE,
    INSTRUCTION_KIND_BINARY_HALF_OPEN_RANGE,
    INSTRUCTION_KIND_BINARY_ANNOT_ACCESS,
    INSTRUCTION_KIND_BINARY_BITWISE_AND,
    INSTRUCTION_KIND_BINARY_BITWISE_OR,
    INSTRUCTION_KIND_BINARY_BITWISE_LEFT_SHIFT,
    INSTRUCTION_KIND_MOVE = 20,
    INSTRUCTION_KIND_CONST_LOAD = 21,
    INSTRUCTION_KIND_NEW_STRUCTURE,
    INSTRUCTION_KIND_MAP_STORE = 23,
    INSTRUCTION_KIND_NEW_ARRAY = 25,
    INSTRUCTION_KIND_ARRAY_STORE = 26,
    INSTRUCTION_KIND_ARRAY_LOAD = 27,
    INSTRUCTION_KIND_TYPE_CAST = 29,
    INSTRUCTION_KIND_TYPE_TEST = 31,
    INSTRUCTION_KIND_NEW_TYPEDESC = 52,
    INSTRUCTION_KIND_BINARY_ADD = 61,
    INSTRUCTION_KIND_BINARY_SUB,
    INSTRUCTION_KIND_BINARY_MUL,
    INSTRUCTION_KIND_BINARY_DIV,
    INSTRUCTION_KIND_BINARY_MOD,
    INSTRUCTION_KIND_BINARY_EQUAL = 66,
    INSTRUCTION_KIND_BINARY_NOT_EQUAL = 67,
    INSTRUCTION_KIND_BINARY_GREATER_THAN = 68,
    INSTRUCTION_KIND_BINARY_GREATER_EQUAL = 69,
    INSTRUCTION_KIND_BINARY_LESS_THAN = 70,
    INSTRUCTION_KIND_BINARY_LESS_EQUAL = 71,
    INSTRUCTION_KIND_UNARY_NOT = 81,
    INSTRUCTION_KIND_UNARY_NEG,
    INSTRUCTION_KIND_BINARY_BITWISE_XOR = 85,
    INSTRUCTION_KIND_BINARY_BITWISE_UNSIGNED_RIGHT_SHIFT
};

class AbstractInstruction : public Debuggable {
  private:
    Operand lhsOp;
    std::shared_ptr<BasicBlock> parentBB;

  protected:
    const Operand &getLhsOperand() const;
    const Function &getFunctionRef() const;
    const Package &getPackageRef() const;
    Function &getFunctionMutableRef() const;
    Package &getPackageMutableRef() const;

  public:
    AbstractInstruction() = delete;
    AbstractInstruction(const Operand &lOp, std::shared_ptr<BasicBlock> parentBB);
    virtual ~AbstractInstruction() = default;
};

} // namespace nballerina

#endif //!__ABSTRACTINSN__H__
