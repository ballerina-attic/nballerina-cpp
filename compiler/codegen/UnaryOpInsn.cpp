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

#include "UnaryOpInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include <llvm-c/Core.h>
#include <string>

namespace nballerina {

UnaryOpInsn::UnaryOpInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &rhs)
    : NonTerminatorInsn(lhs, currentBB), rhsOp(rhs) {}

void UnaryOpInsn::setInstKind(InstructionKind kind) { this->kind = kind; }

void UnaryOpInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {

    const auto &funcObj = getFunctionRef();
    const auto &lhsOp = getLhsOperand();
    std::string lhsTmpName = lhsOp.getName() + "_temp";
    LLVMValueRef lhsRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(lhsOp, module));
    LLVMValueRef rhsOpref = llvm::wrap(funcObj.createTempVariable(rhsOp, module, builder));

    switch (kind) {
    case INSTRUCTION_KIND_UNARY_NOT: {
        LLVMValueRef retVal = LLVMBuildNot(llvm::wrap(&builder), rhsOpref, lhsTmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), retVal, lhsRef);
        break;
    }
    case INSTRUCTION_KIND_UNARY_NEG: {
        LLVMValueRef retVal = LLVMBuildNeg(llvm::wrap(&builder), rhsOpref, lhsTmpName.c_str());
        LLVMBuildStore(llvm::wrap(&builder), retVal, lhsRef);
        break;
    }
    default:
        llvm_unreachable("");
    }
}

} // namespace nballerina
