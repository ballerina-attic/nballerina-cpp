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
#include <llvm-c/Core.h>
#include <string>

using namespace std;

namespace nballerina {
BinaryOpInsn::BinaryOpInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &rhsOp1,
                           const Operand &rhsOp2)
    : NonTerminatorInsn(lhs, std::move(currentBB)), rhsOp1(rhsOp1), rhsOp2(rhsOp2), kind{} {}

void BinaryOpInsn::setInstKind(InstructionKind kind) { this->kind = kind; }

void BinaryOpInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {

    auto &funcObj = getFunctionMutableRef();
    string lhsName = getLhsOperand().getName();
    string lhstmpName = lhsName + "_temp";
    LLVMValueRef lhsRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMValueRef rhsOp1ref = llvm::wrap(funcObj.createTempVariable(rhsOp1, module, builder));
    LLVMValueRef rhsOp2ref = llvm::wrap(funcObj.createTempVariable(rhsOp2, module, builder));

    switch (kind) {
    case INSTRUCTION_KIND_BINARY_ADD: {
        LLVMValueRef ifReturn = LLVMBuildAdd(llvm::wrap(&builder), rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_SUB: {
        LLVMValueRef ifReturn = LLVMBuildSub(llvm::wrap(&builder), rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_MUL: {
        LLVMValueRef ifReturn = LLVMBuildMul(llvm::wrap(&builder), rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_DIV: {
        LLVMValueRef ifReturn = LLVMBuildUDiv(llvm::wrap(&builder), rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_MOD: {
        LLVMValueRef ifReturn = LLVMBuildURem(llvm::wrap(&builder), rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), ifReturn, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_THAN: {
        LLVMValueRef ifReturn = LLVMBuildICmp(llvm::wrap(&builder), LLVMIntUGT, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        funcObj.insertBranchComparisonValue(lhsName, llvm::unwrap(ifReturn));
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_EQUAL: {
        LLVMValueRef ifReturn = LLVMBuildICmp(llvm::wrap(&builder), LLVMIntUGE, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        funcObj.insertBranchComparisonValue(lhsName, llvm::unwrap(ifReturn));
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_THAN: {
        LLVMValueRef ifReturn = LLVMBuildICmp(llvm::wrap(&builder), LLVMIntULT, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        funcObj.insertBranchComparisonValue(lhsName, llvm::unwrap(ifReturn));
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_EQUAL: {
        LLVMValueRef ifReturn = LLVMBuildICmp(llvm::wrap(&builder), LLVMIntULE, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        funcObj.insertBranchComparisonValue(lhsName, llvm::unwrap(ifReturn));
        break;
    }
    case INSTRUCTION_KIND_BINARY_EQUAL: {
        LLVMValueRef ifReturn = LLVMBuildICmp(llvm::wrap(&builder), LLVMIntEQ, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        funcObj.insertBranchComparisonValue(lhsName, llvm::unwrap(ifReturn));
        break;
    }
    case INSTRUCTION_KIND_BINARY_NOT_EQUAL: {
        LLVMValueRef ifReturn = LLVMBuildICmp(llvm::wrap(&builder), LLVMIntNE, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        funcObj.insertBranchComparisonValue(lhsName, llvm::unwrap(ifReturn));
        break;
    }
    case INSTRUCTION_KIND_BINARY_BITWISE_XOR: {
        LLVMValueRef ifReturn = LLVMBuildXor(llvm::wrap(&builder), rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), ifReturn, lhsRef);
        break;
    }
    default:
        llvm_unreachable("");
    }
}
} // namespace nballerina
