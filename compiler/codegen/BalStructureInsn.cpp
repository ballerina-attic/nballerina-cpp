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

#include "BalStructureInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"
#include <iostream>

using namespace std;
using namespace llvm;

namespace nballerina {

StructureInsn::StructureInsn(Operand *lOp, BasicBlock *currentBB, [[maybe_unused]] Operand *_rhsOp)
    : NonTerminatorInsn(lOp, currentBB) {}

void StructureInsn::translate(LLVMModuleRef &modRef) {

    Function *funcObj = getFunction();
    // Find Variable corresponding to lhs to determine structure and member type
    Variable *lhsVar = funcObj->getLocalOrGlobalVariable(getLHS());

    // Determine structure type
    TypeTag structType = lhsVar->getTypeDecl()->getTypeTag();

    // Only handle Map type
    if (structType != TYPE_TAG_MAP) {
        std::cerr << "Non MAP type structs are currently not supported" << std::endl;
        llvm_unreachable("");
    }
    mapInsnTranslate(lhsVar, modRef);
}

void StructureInsn::mapInsnTranslate(Variable *lhsVar, LLVMModuleRef &modRef) {

    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    auto *mapTypeDelare = dynamic_cast<MapTypeDecl *>(lhsVar->getTypeDecl());

    // Get member type
    TypeTag memberTypeTag = mapTypeDelare->getMemberTypeTag();
    // Only handle Int type
    if (memberTypeTag != TYPE_TAG_INT) {
        std::cerr << "Non INT type maps are currently not supported" << std::endl;
        llvm_unreachable("");
    }

    // Codegen for Map of Int type
    LLVMValueRef newMapIntFunc = getNewMapIntDeclaration(modRef);
    LLVMValueRef newMapIntRef = LLVMBuildCall(builder, newMapIntFunc, nullptr, 0, "");
    LLVMBuildStore(builder, newMapIntRef, lhsOpRef);
}

// Declaration for new map<int> function
LLVMValueRef StructureInsn::getNewMapIntDeclaration(LLVMModuleRef &modRef) {

    LLVMValueRef newMapIntFunc = getPackage()->getFunctionRef("map_new_int");
    if (newMapIntFunc != nullptr) {
        return newMapIntFunc;
    }
    LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMTypeRef funcType = LLVMFunctionType(memPtrType, nullptr, 0, 0);
    newMapIntFunc = LLVMAddFunction(modRef, "map_new_int", funcType);
    getPackage()->addFunctionRef("map_new_int", newMapIntFunc);
    return newMapIntFunc;
}

} // namespace nballerina
