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

#include "ConditionBrInsn.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Operand.h"
#include "Types.h"
#include "Variable.h"

namespace nballerina {

ConditionBrInsn::ConditionBrInsn(const Operand &lhs, BasicBlock &currentBB, std::string ifBBID, std::string elseBBID)
    : TerminatorInsn(lhs, currentBB, "", true), ifBBID(std::move(ifBBID)), elseBBID(std::move(elseBBID)) {
    kind = INSTRUCTION_KIND_CONDITIONAL_BRANCH;
}

const std::string &ConditionBrInsn::getIfThenBBID() const { return ifBBID; }
const std::string &ConditionBrInsn::getElseBBID() const { return elseBBID; }
void ConditionBrInsn::setIfThenBB(std::weak_ptr<BasicBlock> bb) { ifThenBB = std::move(bb); }
void ConditionBrInsn::setElseBB(std::weak_ptr<BasicBlock> bb) { elseBB = std::move(bb); }

void ConditionBrInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {

    const auto &funcRef = getFunctionRef();
    std::string lhsName = getLhsOperand().getName();
    auto *brCondition = funcRef.getLLVMValueForBranchComparison(lhsName);
    if (brCondition == nullptr) {
        auto *lhsTemp = funcRef.createTempVariable(getLhsOperand(), module, builder);
        brCondition = builder.CreateIsNotNull(lhsTemp, lhsName);
    }
    assert(!ifThenBB.expired());
    assert(!elseBB.expired());
    builder.CreateCondBr(brCondition, ifThenBB.lock()->getLLVMBBRef(), elseBB.lock()->getLLVMBBRef());
}

} // namespace nballerina
