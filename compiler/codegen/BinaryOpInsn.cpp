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

#include "BinaryOpInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include <string>

namespace nballerina {
BinaryOpInsn::BinaryOpInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &rhsOp1, const Operand &rhsOp2)
    : NonTerminatorInsn(lhs, currentBB), rhsOp1(rhsOp1), rhsOp2(rhsOp2), kind{} {}

void BinaryOpInsn::setInstKind(InstructionKind kind) { this->kind = kind; }

void BinaryOpInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {

    auto &funcObj = getFunctionMutableRef();
    const std::string lhsName = getLhsOperand().getName();
    const std::string lhstmpName = lhsName + "_temp";
    auto *lhsRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *rhsOp1ref = funcObj.createTempVariable(rhsOp1, module, builder);
    auto *rhsOp2ref = funcObj.createTempVariable(rhsOp2, module, builder);

    switch (kind) {
    case INSTRUCTION_KIND_BINARY_ADD: {
        auto *ifReturn = builder.CreateAdd(rhsOp1ref, rhsOp2ref, lhstmpName);
        builder.CreateStore(ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_SUB: {
        auto *ifReturn = builder.CreateSub(rhsOp1ref, rhsOp2ref, lhstmpName);
        builder.CreateStore(ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_MUL: {
        auto *ifReturn = builder.CreateMul(rhsOp1ref, rhsOp2ref, lhstmpName);
        builder.CreateStore(ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_DIV: {
        auto *ifReturn = builder.CreateUDiv(rhsOp1ref, rhsOp2ref, lhstmpName);
        builder.CreateStore(ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_MOD: {
        auto *ifReturn = builder.CreateURem(rhsOp1ref, rhsOp2ref, lhstmpName);
        builder.CreateStore(ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_THAN: {
        auto *ifReturn = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SGT, rhsOp1ref, rhsOp2ref, lhstmpName);
        funcObj.insertBranchComparisonValue(lhsName, ifReturn);

        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_EQUAL: {
        auto *ifReturn = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SGE, rhsOp1ref, rhsOp2ref, lhstmpName);
        funcObj.insertBranchComparisonValue(lhsName, ifReturn);
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_THAN: {
        auto *ifReturn = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, rhsOp1ref, rhsOp2ref, lhstmpName);
        funcObj.insertBranchComparisonValue(lhsName, ifReturn);
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_EQUAL: {
        auto *ifReturn = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLE, rhsOp1ref, rhsOp2ref, lhstmpName);
        funcObj.insertBranchComparisonValue(lhsName, ifReturn);
        break;
    }
    case INSTRUCTION_KIND_BINARY_EQUAL: {
        auto *ifReturn = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, rhsOp1ref, rhsOp2ref, lhstmpName);
        funcObj.insertBranchComparisonValue(lhsName, ifReturn);
        break;
    }
    case INSTRUCTION_KIND_BINARY_NOT_EQUAL: {
        auto *ifReturn = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, rhsOp1ref, rhsOp2ref, lhstmpName);
        funcObj.insertBranchComparisonValue(lhsName, ifReturn);
        break;
    }
    case INSTRUCTION_KIND_BINARY_BITWISE_XOR: {
        auto *ifReturn = builder.CreateXor(rhsOp1ref, rhsOp2ref, lhstmpName);
        builder.CreateStore(ifReturn, lhsRef);
        break;
    }
    default:
        llvm_unreachable("");
    }
}
} // namespace nballerina
