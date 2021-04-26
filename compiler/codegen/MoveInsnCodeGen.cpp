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

#include "Function.h"
#include "MoveInsn.h"
#include "NonTerminatorInsnCodeGen.h"
#include "Operand.h"
#include "Package.h"
#include "Variable.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(class MoveInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {

    auto *lhsRef = parentGenerator.getLLVMLocalOrGlobalVar(obj.getLhsOperand(), module);
    auto *rhsVarOpRef = parentGenerator.createTempVariable(obj.rhsOp, module, builder);
    builder.CreateStore(rhsVarOpRef, lhsRef);
}

} // namespace nballerina
