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

#include "codegen/NonTerminatorInsnCodeGen.h"
#include "codegen/CodeGenUtils.h"
#include "bir/Function.h"
#include "bir/MapInsns.h"
#include "bir/Operand.h"
#include "bir/StructureInsn.h"
#include "bir/Types.h"
#include "bir/Variable.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::mapCreateTranslate(StructureInsn &obj, const Variable &lhsVar,
                                                  llvm::IRBuilder<> &builder) {

    auto *lhsOpRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    const auto &mapType = lhsVar.getType();
    TypeTag memberTypeTag = mapType.getMemberTypeTag();
    Type::checkMapSupport(memberTypeTag);

    auto newMapIntFunc = CodeGenUtils::getNewMapInitFunc(moduleGenerator.getModule(), memberTypeTag);
    auto *newMapIntRef = builder.CreateCall(newMapIntFunc);
    builder.CreateStore(newMapIntRef, lhsOpRef);
}

void NonTerminatorInsnCodeGen::mapInitTranslate(StructureInsn &obj, const Variable &lhsVar,
                                                llvm::IRBuilder<> &builder) {

    TypeTag memTypeTag = lhsVar.getType().getMemberTypeTag();
    Type::checkMapSupport(memTypeTag);

    auto mapStoreFunc = CodeGenUtils::getMapStoreFunc(moduleGenerator.getModule(), memTypeTag);
    auto mapSpreadFieldFunc = CodeGenUtils::getMapSpreadFieldInitFunc(moduleGenerator.getModule());
    for (const auto &initValue : obj.initValues) {
        const auto &initstruct = initValue.getInitValStruct();
        if (initValue.getKind() == Spread_Field_Kind) {
            const auto &expr = std::get<MapConstruct::SpreadField>(initstruct).getExpr();
            auto *lhsTmp = functionGenerator.createTempVal(obj.lhsOp, builder);
            auto *exprTmp = functionGenerator.createTempVal(expr, builder);
            builder.CreateCall(mapSpreadFieldFunc, llvm::ArrayRef<llvm::Value *>({lhsTmp, exprTmp}));
            continue;
        }
        // For Key_Value_Kind
        const auto &keyVal = std::get<MapConstruct::KeyValue>(initstruct);
        llvm::Value *mapValue = Type::isSmartStructType(memTypeTag)
                                    ? functionGenerator.getLocalOrGlobalVal(keyVal.getValue())
                                    : functionGenerator.createTempVal(keyVal.getValue(), builder);
        builder.CreateCall(mapStoreFunc, llvm::ArrayRef<llvm::Value *>(
                                             {functionGenerator.createTempVal(obj.lhsOp, builder),
                                              functionGenerator.createTempVal(keyVal.getKey(), builder), mapValue}));
    }
}

void NonTerminatorInsnCodeGen::visit(StructureInsn &obj, llvm::IRBuilder<> &builder) {

    // Find Variable corresponding to lhs to determine structure and member type
    const auto &lhsVar = obj.getFunctionRef().getLocalOrGlobalVariable(obj.lhsOp);
    TypeTag structType = lhsVar.getType().getTypeTag();
    // Only handle Map type
    if (structType != TYPE_TAG_MAP) {
        llvm_unreachable("Only Map type structs are currently supported");
    }

    mapCreateTranslate(obj, lhsVar, builder);
    if (obj.initValues.empty()) {
        return;
    }
    mapInitTranslate(obj, lhsVar, builder);
}

} // namespace nballerina
