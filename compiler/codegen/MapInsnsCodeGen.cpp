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

#include "bir/Function.h"
#include "bir/MapInsns.h"
#include "bir/Operand.h"
#include "bir/Types.h"
#include "bir/Variable.h"
#include "codegen/CodeGenUtils.h"
#include "codegen/NonTerminatorInsnCodeGen.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(MapStoreInsn &obj, llvm::IRBuilder<> &builder) {
    const auto &lhsVar = obj.getFunctionRef().getLocalOrGlobalVariable(obj.lhsOp);
    auto memberTypeTag = lhsVar.getType().getMemberTypeTag();
    Type::checkMapSupport(memberTypeTag);
    llvm::Value *mapValue = functionGenerator.createTempVal(obj.rhsOp, builder);
    builder.CreateCall(CodeGenUtils::getMapStoreFunc(moduleGenerator.getModule()),
                       llvm::ArrayRef<llvm::Value *>({functionGenerator.createTempVal(obj.lhsOp, builder),
                                                      functionGenerator.createTempVal(obj.keyOp, builder), mapValue}));
}

void NonTerminatorInsnCodeGen::visit(MapLoadInsn &obj, llvm::IRBuilder<> &builder) {
    TypeTag memTypeTag = obj.getFunctionRef().getLocalOrGlobalVariable(obj.rhsOp).getType().getMemberTypeTag();
    auto *outParamType = CodeGenUtils::getLLVMTypeOfType(memTypeTag, moduleGenerator.getModule());

    auto *lhs = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    auto *outParam = builder.CreateAlloca(outParamType);
    auto *rhsTemp = functionGenerator.createTempVal(obj.rhsOp, builder);
    auto *keyTemp = functionGenerator.createTempVal(obj.keyOp, builder);
    auto mapLoadFunction = CodeGenUtils::getMapLoadFunc(moduleGenerator.getModule());

    [[maybe_unused]] auto *retVal =
        builder.CreateCall(mapLoadFunction, llvm::ArrayRef<llvm::Value *>({rhsTemp, keyTemp, outParam}));

    // TODO check retVal and branch
    // if retVal is true
    auto *balValue = CodeGenUtils::createBalValue(moduleGenerator.getModule(), builder, outParam, Type(memTypeTag, ""));
    builder.CreateStore(balValue, lhs);

    // else
    // moduleGenerator.createBalValue(modRef, getPackageRef().getGlobalNilVar(), Type(TYPE_TAG_NIL,
    // ""), lhs);
}

} // namespace nballerina
