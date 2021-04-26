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

#include "CodeGenUtils.h"
#include "Function.h"
#include "MapInsns.h"
#include "NonTerminatorInsnCodeGen.h"
#include "Operand.h"
#include "Package.h"
#include "StructureInsn.h"
#include "TypeUtils.h"
#include "Types.h"
#include "Variable.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::mapCreateTranslate(StructureInsn &obj, const Variable &lhsVar, llvm::Module &module,
                                                  llvm::IRBuilder<> &builder) {

    auto *lhsOpRef = parentGenerator.getLLVMLocalOrGlobalVar(obj.getLhsOperand(), module);
    const auto &mapType = lhsVar.getType();
    TypeTag memberTypeTag = mapType.getMemberTypeTag();
    TypeUtils::checkMapSupport(memberTypeTag);

    auto newMapIntFunc = CodeGenUtils::getNewMapInitFunc(module, memberTypeTag);
    auto *newMapIntRef = builder.CreateCall(newMapIntFunc);
    builder.CreateStore(newMapIntRef, lhsOpRef);
}

void NonTerminatorInsnCodeGen::mapInitTranslate(StructureInsn &obj, const Variable &lhsVar, llvm::Module &module,
                                                llvm::IRBuilder<> &builder) {

    TypeTag memTypeTag = lhsVar.getType().getMemberTypeTag();
    TypeUtils::checkMapSupport(memTypeTag);

    auto mapStoreFunc = CodeGenUtils::getMapStoreFunc(module, memTypeTag);
    auto mapSpreadFieldFunc = CodeGenUtils::getMapSpreadFieldInitFunc(module);
    for (const auto &initValue : obj.initValues) {
        const auto &initstruct = initValue.getInitValStruct();
        if (initValue.getKind() == Spread_Field_Kind) {
            const auto &expr = std::get<MapConstruct::SpreadField>(initstruct).getExpr();
            auto *lhsTmp = parentGenerator.createTempVariable(obj.getLhsOperand(), module, builder);
            auto *exprTmp = parentGenerator.createTempVariable(expr, module, builder);
            builder.CreateCall(mapSpreadFieldFunc, llvm::ArrayRef<llvm::Value *>({lhsTmp, exprTmp}));
            continue;
        }
        // For Key_Value_Kind
        const auto &keyVal = std::get<MapConstruct::KeyValue>(initstruct);
        llvm::Value *mapValue = Type::isSmartStructType(memTypeTag)
                                    ? parentGenerator.getLLVMLocalOrGlobalVar(keyVal.getValue(), module)
                                    : parentGenerator.createTempVariable(keyVal.getValue(), module, builder);
        builder.CreateCall(mapStoreFunc,
                           llvm::ArrayRef<llvm::Value *>(
                               {parentGenerator.createTempVariable(obj.getLhsOperand(), module, builder),
                                parentGenerator.createTempVariable(keyVal.getKey(), module, builder), mapValue}));
    }
}

void NonTerminatorInsnCodeGen::visit(StructureInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {

    // Find Variable corresponding to lhs to determine structure and member type
    const auto &lhsVar = obj.getFunctionRef().getLocalOrGlobalVariable(obj.getLhsOperand());
    TypeTag structType = lhsVar.getType().getTypeTag();
    // Only handle Map type
    if (structType != TYPE_TAG_MAP) {
        llvm_unreachable("Only Map type structs are currently supported");
    }

    mapCreateTranslate(obj, lhsVar, module, builder);
    if (obj.initValues.empty()) {
        return;
    }
    mapInitTranslate(obj, lhsVar, module, builder);
}

} // namespace nballerina
