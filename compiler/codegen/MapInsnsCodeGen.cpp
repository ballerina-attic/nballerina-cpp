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
#include "CodeGenUtils.h"
#include "Function.h"
#include "NonTerminatorInsnCodeGen.h"
#include "Operand.h"
#include "Package.h"
#include "TypeUtils.h"
#include "Types.h"
#include "Variable.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(MapStoreInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(obj.getLhsOperand());
    auto memberTypeTag = lhsVar.getType().getMemberTypeTag();
    TypeUtils::checkMapSupport(memberTypeTag);
    llvm::Value *mapValue = Type::isSmartStructType(memberTypeTag)
                                ? funcObj.getLLVMLocalOrGlobalVar(obj.rhsOp, module)
                                : funcObj.createTempVariable(obj.rhsOp, module, builder);
    builder.CreateCall(
        CodeGenUtils::getMapStoreFunc(module, memberTypeTag),
        llvm::ArrayRef<llvm::Value *>({funcObj.createTempVariable(obj.getLhsOperand(), module, builder),
                                       funcObj.createTempVariable(obj.keyOp, module, builder), mapValue}));
}

void NonTerminatorInsnCodeGen::visit(MapLoadInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    TypeTag memTypeTag = funcObj.getLocalOrGlobalVariable(obj.rhsOp).getType().getMemberTypeTag();
    auto *outParamType = CodeGenUtils::getLLVMTypeOfType(memTypeTag, module);

    auto *lhs = funcObj.getLLVMLocalOrGlobalVar(obj.getLhsOperand(), module);
    auto *outParam = builder.CreateAlloca(outParamType);
    auto *rhsTemp = funcObj.createTempVariable(obj.rhsOp, module, builder);
    auto *keyTemp = funcObj.createTempVariable(obj.keyOp, module, builder);
    auto mapLoadFunction = CodeGenUtils::getMapLoadFunc(module, memTypeTag);

    [[maybe_unused]] auto *retVal =
        builder.CreateCall(mapLoadFunction, llvm::ArrayRef<llvm::Value *>({rhsTemp, keyTemp, outParam}));
    // TODO check retVal and branch
    // if retVal is true
    if (Type::isSmartStructType(memTypeTag)) {
        auto *outParamTemp = builder.CreateLoad(outParam);
        builder.CreateStore(outParamTemp, lhs);
    } else {
        obj.getPackageMutableRef().storeValueInSmartStruct(module, builder, outParam, Type(memTypeTag, ""), lhs);
    }
    // else
    // getFunctionMutableRef().storeValueInSmartStruct(modRef, getPackageRef().getGlobalNilVar(), Type(TYPE_TAG_NIL,
    // ""), lhs);
}

} // namespace nballerina
