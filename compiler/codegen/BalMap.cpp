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

#include "BalFunction.h"
#include "BalMapInsns.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"
#include <iostream>

using namespace std;

namespace nballerina {

// new Map Instruction and Codegen logic are in the llvmStructure.cpp

MapStoreInsn::MapStoreInsn(Operand *lOp, BasicBlock *currentBB, Operand *KOp, Operand *rOp)
    : NonTerminatorInsn(lOp, currentBB), keyOp(KOp), rhsOp(rOp) {}

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();

    // Find Variable corresponding to lhs to determine member type
    Variable *lhsVar = funcObj->getLocalOrGlobalVariable(getLHS());
    MapTypeDecl *mapTypeDelare = static_cast<MapTypeDecl *>(lhsVar->getTypeDecl());
    TypeTag memberTypeTag = mapTypeDelare->getMemberTypeTag();

    // Only handle Int type
    if (memberTypeTag != TYPE_TAG_INT) {
        std::cerr << "Non INT type maps are currently not supported" << std::endl;
        llvm_unreachable("Unknown Type");
    }

    // Codegen for Map of Int type store
    LLVMValueRef mapStoreFunc = getMapIntStoreDeclaration(modRef);
    LLVMValueRef lhsOpTempRef = funcObj->getTempLocalVariable(getLHS());
    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef keyRef = funcObj->getTempLocalVariable(keyOp);

    LLVMValueRef *argOpValueRef = new LLVMValueRef[3];
    argOpValueRef[0] = lhsOpTempRef;
    argOpValueRef[1] = keyRef;
    argOpValueRef[2] = rhsOpRef;

    LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

// Declaration for map<int> type store function
LLVMValueRef MapStoreInsn::getMapIntStoreDeclaration(LLVMModuleRef &modRef) {

    LLVMValueRef mapStoreFunc = getPackage()->getFunctionRef("map_store_int");
    if (mapStoreFunc)
        return mapStoreFunc;

    LLVMTypeRef *paramTypes = new LLVMTypeRef[3];
    LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
    LLVMTypeRef charArrayPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    paramTypes[0] = memPtrType;
    paramTypes[1] = charArrayPtrType;
    paramTypes[2] = int32PtrType;
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    mapStoreFunc = LLVMAddFunction(modRef, "map_store_int", funcType);
    getPackage()->addFunctionRef("map_store_int", mapStoreFunc);
    return mapStoreFunc;
}

} // namespace nballerina
