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

#include "MapInsns.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"

using namespace std;

namespace nballerina {

// new Map Instruction and Codegen logic are in the llvmStructure.cpp

MapStoreInsn::MapStoreInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp,
                           const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    auto lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    assert(lhsVar.has_value());
    auto memberTypeTag = lhsVar->getType().getMemberTypeTag();

    // Only handle Int type
    if (memberTypeTag != TYPE_TAG_INT) {
        llvm_unreachable("Only int type maps are currently supported");
    }

    // Codegen for map<int> type store
    codeGenMapStore(funcObj.getLLVMBuilder(), getPackageMutableRef().getMapIntStoreDeclaration(modRef),
                    funcObj.createTempVariable(getLhsOperand()), funcObj.createTempVariable(keyOp),
                    funcObj.getLLVMLocalOrGlobalVar(rhsOp));
}

// Generic map store static function
void MapStoreInsn::codeGenMapStore(LLVMBuilderRef builder, LLVMValueRef mapStoreFunc, LLVMValueRef map,
                                   LLVMValueRef key, LLVMValueRef value) {
    LLVMValueRef argOpValueRef[] = {map, key, value};
    LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
