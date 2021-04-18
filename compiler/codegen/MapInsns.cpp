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

MapStoreInsn::MapStoreInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &KOp,
                           const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

void MapStoreInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    auto memberTypeTag = lhsVar.getType().getMemberTypeTag();

    // Only handle Int type
    TypeUtils::checkMapSupport(memberTypeTag);

    LLVMValueRef mapValue;
    if (Type::isSmartStructType(memberTypeTag)) {
        mapValue = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(rhsOp, module));
    } else {
        mapValue = llvm::wrap(funcObj.createTempVariable(rhsOp, module, builder));
    }
    // Codegen for map<int> type store
    codeGenMapStore(llvm::wrap(&builder),
                    llvm::wrap(getPackageRef().getMapStoreDeclaration(module, memberTypeTag).getCallee()),
                    llvm::wrap(funcObj.createTempVariable(getLhsOperand(), module, builder)),
                    llvm::wrap(funcObj.createTempVariable(keyOp, module, builder)), mapValue);
}

// Generic map store static function
void MapStoreInsn::codeGenMapStore(LLVMBuilderRef builder, LLVMValueRef mapStoreFunc, LLVMValueRef map,
                                   LLVMValueRef key, LLVMValueRef value) {
    LLVMValueRef argOpValueRef[] = {map, key, value};
    LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

MapLoadInsn::MapLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &KOp,
                         const Operand &rOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), keyOp(KOp), rhsOp(rOp) {}

void MapLoadInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {

    const auto &funcObj = getFunctionRef();
    TypeTag memTypeTag = funcObj.getLocalOrGlobalVariable(rhsOp).getType().getMemberTypeTag();
    auto *outParamType = getPackageRef().getLLVMTypeOfType(memTypeTag, module);

    LLVMValueRef lhs = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMValueRef outParam = LLVMBuildAlloca(llvm::wrap(&builder), llvm::wrap(outParamType), "_out_param");
    LLVMValueRef params[] = {llvm::wrap(funcObj.createTempVariable(rhsOp, module, builder)),
                             llvm::wrap(funcObj.createTempVariable(keyOp, module, builder)), outParam};

    auto modRef = llvm::wrap(&module);
    [[maybe_unused]] LLVMValueRef retVal =
        LLVMBuildCall(llvm::wrap(&builder),
                      getMapLoadDeclaration(modRef, llvm::wrap(llvm::PointerType::get(outParamType, 0)),
                                            Type::getNameOfType(memTypeTag)),
                      params, 3, "");

    // TODO check retVal and branch
    // if retVal is true
    if (Type::isSmartStructType(memTypeTag)) {
        LLVMValueRef outParamTemp = LLVMBuildLoad(llvm::wrap(&builder), outParam, "");
        LLVMBuildStore(llvm::wrap(&builder), outParamTemp, lhs);
    } else {
        getFunctionMutableRef().storeValueInSmartStruct(module, builder, llvm::unwrap(outParam), Type(memTypeTag, ""),
                                                        llvm::unwrap(lhs));
    }
    // else
    // getFunctionMutableRef().storeValueInSmartStruct(modRef, getPackageRef().getGlobalNilVar(), Type(TYPE_TAG_NIL,
    // ""), lhs);
}

LLVMValueRef MapLoadInsn::getMapLoadDeclaration(LLVMModuleRef &modRef, LLVMTypeRef outParamType, std::string typeName) {
    const std::string funcName = "map_load_" + typeName;
    auto *module = llvm::unwrap(modRef);
    auto *functionRef = module->getFunction(funcName);
    if (functionRef != nullptr) {
        return llvm::wrap(functionRef);
    }

    LLVMTypeRef funcRetType = LLVMInt8Type();
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMPointerType(LLVMInt8Type(), 0), outParamType};
    LLVMTypeRef funcType = LLVMFunctionType(funcRetType, paramTypes, 3, 0);
    return LLVMAddFunction(modRef, funcName.c_str(), funcType);
}

} // namespace nballerina
