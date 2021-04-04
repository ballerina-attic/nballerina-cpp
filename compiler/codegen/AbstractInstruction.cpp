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

#include "interfaces/AbstractInstruction.h"
#include "BasicBlock.h"
#include "Function.h"

namespace nballerina {

AbstractInstruction::AbstractInstruction(const Operand &lOp, std::shared_ptr<BasicBlock> parentBB)
    : lhsOp(lOp), parentBB(std::move(parentBB)) {}

const Operand &AbstractInstruction::getLhsOperand() const { return lhsOp; }
const Function &AbstractInstruction::getFunctionRef() const { return parentBB->getParentFunctionRef(); }
const Package &AbstractInstruction::getPackageRef() const { return parentBB->getParentFunctionRef().getPackageRef(); }
Function &AbstractInstruction::getFunctionMutableRef() const { return *parentBB->getFunctionSharedObj(); }
Package &AbstractInstruction::getPackageMutableRef() const {
    return parentBB->getFunctionSharedObj()->getPackageMutableRef();
}

} // namespace nballerina
