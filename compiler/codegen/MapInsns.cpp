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
#include "TypeUtils.h"
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
    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    auto memberTypeTag = lhsVar.getType().getMemberTypeTag();

    // Only handle Int type
    TypeUtils::checkMapSupport(memberTypeTag);

    LLVMValueRef mapValue;
    if (Type::isSmartStructType(memberTypeTag)) {
        mapValue = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    } else {
        mapValue = funcObj.createTempVariable(rhsOp);
    }
    // Codegen for map<int> type store
    codeGenMapStore(funcObj.getLLVMBuilder(), getPackageMutableRef().getMapStoreDeclaration(modRef, memberTypeTag),
                    funcObj.createTempVariable(getLhsOperand()), funcObj.createTempVariable(keyOp), mapValue);
}

// Generic map store static function
void MapStoreInsn::codeGenMapStore(LLVMBuilderRef builder, LLVMValueRef mapStoreFunc, LLVMValueRef map,
                                   LLVMValueRef key, LLVMValueRef value) {
    LLVMValueRef argOpValueRef[] = {map, key, value};
    LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

MapLoadInsn::MapLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp,
                         const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

void MapLoadInsn::translate(LLVMModuleRef &modRef) {

    const auto &funcObj = getFunctionRef();
    auto builder = funcObj.getLLVMBuilder();
    TypeTag memTypeTag = funcObj.getLocalOrGlobalVariable(rhsOp).getType().getMemberTypeTag();
    LLVMTypeRef outParamType = getPackageRef().getLLVMTypeOfType(memTypeTag);

    LLVMValueRef lhs = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef outParam = LLVMBuildAlloca(builder, outParamType, "_out_param");
    LLVMValueRef params[] = {funcObj.createTempVariable(rhsOp), funcObj.createTempVariable(keyOp), outParam};

    [[maybe_unused]] LLVMValueRef retVal = LLVMBuildCall(
        builder, getMapLoadDeclaration(modRef, LLVMPointerType(outParamType, 0), Type::getNameOfType(memTypeTag)),
        params, 3, "");

    // TODO check retVal and branch
    // if retVal is true
    if (Type::isSmartStructType(memTypeTag)) {
        LLVMValueRef outParamTemp = LLVMBuildLoad(builder, outParam, "");
        LLVMBuildStore(builder, outParamTemp, lhs);
    } else {
        getFunctionMutableRef().storeValueInSmartStruct(modRef, outParam, Type(memTypeTag, ""), lhs);
    }
    // else
    // getFunctionMutableRef().storeValueInSmartStruct(modRef, getPackageRef().getGlobalNilVar(), Type(TYPE_TAG_NIL,
    // ""), lhs);
}

LLVMValueRef MapLoadInsn::getMapLoadDeclaration(LLVMModuleRef &modRef, LLVMTypeRef outParamType, std::string typeName) {
    std::string funcName = "map_load_" + typeName;
    const char *externalFunctionName = funcName.c_str();
    LLVMValueRef mapLoadFunc = getPackageRef().getFunctionRef(funcName);
    if (mapLoadFunc != nullptr) {
        return mapLoadFunc;
    }
    LLVMTypeRef funcRetType = LLVMInt8Type();
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMPointerType(LLVMInt8Type(), 0), outParamType};
    LLVMTypeRef funcType = LLVMFunctionType(funcRetType, paramTypes, 3, 0);
    mapLoadFunc = LLVMAddFunction(modRef, externalFunctionName, funcType);
    getPackageMutableRef().addFunctionRef(funcName, mapLoadFunc);
    return mapLoadFunc;
}

} // namespace nballerina
