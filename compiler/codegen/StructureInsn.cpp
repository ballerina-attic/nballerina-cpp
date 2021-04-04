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

#include "StructureInsn.h"
#include "Function.h"
#include "MapInsns.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"

using namespace std;
using namespace llvm;

namespace nballerina {

StructureInsn::StructureInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB,
                             std::vector<MapConstruct> initValues)
    : NonTerminatorInsn(lhs, std::move(currentBB)), initValues(std::move(initValues)) {}

StructureInsn::StructureInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB)
    : NonTerminatorInsn(lhs, std::move(currentBB)) {}

void StructureInsn::translate(LLVMModuleRef &modRef) {

    const auto &funcObj = getFunctionRef();
    // Find Variable corresponding to lhs to determine structure and member type
    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());

    // Determine structure type
    TypeTag structType = lhsVar.getType().getTypeTag();

    // Only handle Map type
    if (structType != TYPE_TAG_MAP) {
        llvm_unreachable("Only Map type structs are currently supported");
    }

    mapCreateTranslate(lhsVar, modRef);
    if (initValues.empty()) {
        return;
    }
    mapInitTranslate(lhsVar, modRef);
}

void StructureInsn::mapInitTranslate(const Variable &lhsVar, LLVMModuleRef &modRef) {

    // Only handle Int type
    if (lhsVar.getType().getMemberTypeTag() != TYPE_TAG_INT) {
        llvm_unreachable("Only int type maps are currently supported");
    }

    // Codegen for map<int> type store
    LLVMValueRef mapStoreFunc = getPackageMutableRef().getMapIntStoreDeclaration(modRef);
    LLVMValueRef mapSpreadFieldFunc = getPackageMutableRef().getMapSpreadFieldDeclaration(modRef);
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    for (const auto &initValue : initValues) {
        const auto &initstruct = initValue.getInitValStruct();
        if (initValue.getKind() == Spread_Field_Kind) {
            const auto &expr = std::get<MapConstruct::SpreadField>(initstruct).getExpr();
            LLVMValueRef argOpValueRef[] = {funcObj.createTempVariable(getLhsOperand()),
                                            funcObj.createTempVariable(expr)};
            LLVMBuildCall(builder, mapSpreadFieldFunc, argOpValueRef, 2, "");
            continue;
        }
        // For Key_Value_Kind
        const auto &keyVal = std::get<MapConstruct::KeyValue>(initstruct);
        MapStoreInsn::codeGenMapStore(builder, mapStoreFunc, funcObj.createTempVariable(getLhsOperand()),
                                      funcObj.createTempVariable(keyVal.getKey()),
                                      funcObj.getLLVMLocalOrGlobalVar(keyVal.getValue()));
    }
}

void StructureInsn::mapCreateTranslate(const Variable &lhsVar, LLVMModuleRef &modRef) {

    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    const auto &mapType = lhsVar.getType();

    // Get member type
    TypeTag memberTypeTag = mapType.getMemberTypeTag();
    // Only handle Int type
    if (memberTypeTag != TYPE_TAG_INT) {
        llvm_unreachable("Only int type maps are currently supported");
    }

    // Codegen for Map of Int type
    LLVMValueRef newMapIntFunc = getNewMapIntDeclaration(modRef);
    LLVMValueRef newMapIntRef = LLVMBuildCall(builder, newMapIntFunc, nullptr, 0, "");
    LLVMBuildStore(builder, newMapIntRef, lhsOpRef);
}

// Declaration for new map<int> function
LLVMValueRef StructureInsn::getNewMapIntDeclaration(LLVMModuleRef &modRef) {

    LLVMValueRef newMapIntFunc = getPackageRef().getFunctionRef("map_new_int");
    if (newMapIntFunc != nullptr) {
        return newMapIntFunc;
    }
    LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMTypeRef funcType = LLVMFunctionType(memPtrType, nullptr, 0, 0);
    newMapIntFunc = LLVMAddFunction(modRef, "map_new_int", funcType);
    getPackageMutableRef().addFunctionRef("map_new_int", newMapIntFunc);
    return newMapIntFunc;
}

} // namespace nballerina
