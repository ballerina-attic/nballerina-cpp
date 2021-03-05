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

#include "BalConditionBrInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"

using namespace std;

namespace nballerina {

ConditionBrInsn::ConditionBrInsn(Operand *lOp, BasicBlock *currentBB, BasicBlock *_ifThenBB, BasicBlock *_elseBB)
    : TerminatorInsn(lOp, currentBB, nullptr, true), ifThenBB(_ifThenBB), elseBB(_elseBB) {
    kind = INSTRUCTION_KIND_CONDITIONAL_BRANCH;
}

BasicBlock *ConditionBrInsn::getIfThenBB() { return ifThenBB; }
BasicBlock *ConditionBrInsn::getElseBB() { return elseBB; }
void ConditionBrInsn::setIfThenBB(BasicBlock *bb) { ifThenBB = bb; }
void ConditionBrInsn::setElseBB(BasicBlock *bb) { elseBB = bb; }

void ConditionBrInsn::translate([[maybe_unused]] LLVMModuleRef &modRef) {

    LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
    string lhsName = getLHS()->getName();

    LLVMValueRef brCondition = getFunction()->getLLVMValueForBranchComparison(lhsName);
    if (brCondition == nullptr) {
        brCondition = LLVMBuildIsNotNull(builder, getFunction()->getTempLocalVariable(getLHS()), lhsName.c_str());
    }

    LLVMBuildCondBr(builder, brCondition, ifThenBB->getLLVMBBRef(), elseBB->getLLVMBBRef());
}

} // namespace nballerina
