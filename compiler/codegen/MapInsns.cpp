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
    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    auto memberTypeTag = lhsVar.getType().getMemberTypeTag();

    // Only handle Int type
    if (memberTypeTag != TYPE_TAG_INT) {
        llvm_unreachable("Only int type maps are currently supported");
    }

    // Codegen for map<int> type store
    codeGenMapStore(funcObj.getLLVMBuilder(), getPackageMutableRef().getMapIntStoreDeclaration(modRef),
                    funcObj.createTempVariable(getLhsOperand()), funcObj.createTempVariable(keyOp),
                    funcObj.createTempVariable(rhsOp));
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
    
    LLVMValueRef lhs = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef outParam = LLVMBuildAlloca(builder, LLVMInt32Type(), "_out_param");
    LLVMValueRef params[] = {funcObj.createTempVariable(rhsOp), funcObj.createTempVariable(keyOp), outParam};
    
    [[maybe_unused]] LLVMValueRef retVal = LLVMBuildCall(builder, getMapLoadDeclaration(modRef), params, 3, "");

    // TODO check retVal and branch 
    // if retVal is true
    getFunctionMutableRef().storeValueInSmartStruct(modRef, outParam, Type(TYPE_TAG_INT, ""), lhs);
    // else
    // getFunctionMutableRef().storeValueInSmartStruct(modRef, getPackageRef().getGlobalNilVar(), Type(TYPE_TAG_NIL, ""), lhs);
}

LLVMValueRef MapLoadInsn::getMapLoadDeclaration(LLVMModuleRef &modRef) {

    const char *funcName = "map_load_int";
    LLVMValueRef mapLoadFunc = getPackageRef().getFunctionRef(funcName);
    if (mapLoadFunc != nullptr) {
        return mapLoadFunc;
    }
    LLVMTypeRef funcRetType = LLVMInt8Type();
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMPointerType(LLVMInt8Type(), 0),
                                LLVMPointerType(LLVMInt32Type(), 0)};
    LLVMTypeRef funcType = LLVMFunctionType(funcRetType, paramTypes, 3, 0);
    mapLoadFunc = LLVMAddFunction(modRef, funcName, funcType);
    getPackageMutableRef().addFunctionRef(funcName, mapLoadFunc);
    return mapLoadFunc;
}

} // namespace nballerina
