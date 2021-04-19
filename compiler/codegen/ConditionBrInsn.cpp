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
#include "llvm-c/Core.h"

using namespace std;

namespace nballerina {

ConditionBrInsn::ConditionBrInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, std::string ifBBID,
                                 std::string elseBBID)
    : TerminatorInsn(lhs, std::move(currentBB), "", true), ifBBID(std::move(ifBBID)), elseBBID(std::move(elseBBID)) {
    kind = INSTRUCTION_KIND_CONDITIONAL_BRANCH;
}

const std::string &ConditionBrInsn::getIfThenBBID() const { return ifBBID; }
const std::string &ConditionBrInsn::getElseBBID() const { return elseBBID; }
void ConditionBrInsn::setIfThenBB(std::weak_ptr<BasicBlock> bb) { ifThenBB = std::move(bb); }
void ConditionBrInsn::setElseBB(std::weak_ptr<BasicBlock> bb) { elseBB = std::move(bb); }

void ConditionBrInsn::translate(LLVMModuleRef &) {

    const auto &funcRef = getFunctionRef();
    LLVMBuilderRef builder = funcRef.getLLVMBuilder();
    string lhsName = getLhsOperand().getName();

    LLVMValueRef brCondition = funcRef.getLLVMValueForBranchComparison(lhsName);
    if (brCondition == nullptr) {
        brCondition = LLVMBuildIsNotNull(builder, funcRef.createTempVariable(getLhsOperand()), lhsName.c_str());
    }

    assert(!ifThenBB.expired());
    assert(!elseBB.expired());
    LLVMBuildCondBr(builder, brCondition, ifThenBB.lock()->getLLVMBBRef(), elseBB.lock()->getLLVMBBRef());
}

} // namespace nballerina
