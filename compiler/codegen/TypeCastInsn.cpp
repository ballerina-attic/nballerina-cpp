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

#include "TypeCastInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

namespace nballerina {

TypeCastInsn::TypeCastInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, const Operand &rhsOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), rhsOp(rhsOp) {}

void TypeCastInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    LLVMValueRef rhsOpRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(rhsOp, module));
    LLVMValueRef lhsOpRef = llvm::wrap(funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module));
    LLVMTypeRef lhsTypeRef = LLVMTypeOf(lhsOpRef);

    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    const auto &rhsVar = funcObj.getLocalOrGlobalVariable(rhsOp);
    const auto &lhsType = lhsVar.getType();
    auto lhsTypeTag = lhsType.getTypeTag();
    const auto &rhsType = rhsVar.getType();
    auto rhsTypeTag = rhsType.getTypeTag();

    const char *inherentTypeName = "inherentTypeName";

    if (Type::isSmartStructType(rhsTypeTag)) {
        if (Type::isSmartStructType(lhsTypeTag)) {
            LLVMValueRef rhsVarOpRef = llvm::wrap(funcObj.createTempVariable(rhsOp, module, builder));
            LLVMBuildStore(llvm::wrap(&builder), rhsVarOpRef, lhsOpRef);
            return;
        }
        // GEP of last type of smart pointer(original type of any variable(smart pointer))
        LLVMValueRef inherentTypeIdx = LLVMBuildStructGEP(llvm::wrap(&builder), rhsOpRef, 0, inherentTypeName);
        LLVMValueRef inherentTypeLoad = LLVMBuildLoad(llvm::wrap(&builder), inherentTypeIdx, "");
        LLVMValueRef sExt = LLVMBuildSExt(llvm::wrap(&builder), inherentTypeLoad, LLVMInt64Type(), "");

        LLVMValueRef data = LLVMBuildStructGEP(llvm::wrap(&builder), rhsOpRef, 1, "data");
        LLVMValueRef dataLoad = LLVMBuildLoad(llvm::wrap(&builder), data, "");
        LLVMValueRef strTblPtr = llvm::wrap(getPackageRef().getStringBuilderTableGlobalPointer());
        LLVMValueRef strTblLoad = LLVMBuildLoad(llvm::wrap(&builder), strTblPtr, "");
        LLVMValueRef gepOfStr = LLVMBuildInBoundsGEP(llvm::wrap(&builder), strTblLoad, &sExt, 1, "");

        // get the mangled name of the lhs type and store it to string builder table.
        std::string_view lhsTypeName = Type::typeStringMangleName(lhsType);
        getPackageMutableRef().addToStrTable(lhsTypeName); // TODO : get rid of this mutable reference
        int tempRandNum = std::rand() % 1000 + 1;
        LLVMValueRef constValue = LLVMConstInt(LLVMInt64Type(), tempRandNum, 0);
        LLVMValueRef lhsGep = LLVMBuildInBoundsGEP(llvm::wrap(&builder), strTblLoad, &constValue, 1, "");
        // call is_same_type rust function to check LHS and RHS type are same or not.
        LLVMValueRef addedIsSameTypeFunc = getIsSameTypeDeclaration(module, lhsGep, gepOfStr);
        LLVMValueRef paramRefs[] = {lhsGep, gepOfStr};
        LLVMValueRef sameTypeResult = LLVMBuildCall(llvm::wrap(&builder), addedIsSameTypeFunc, paramRefs, 2, "call");
        // creating branch condition using is_same_type() function result.
        [[maybe_unused]] LLVMValueRef brCondition = LLVMBuildIsNotNull(llvm::wrap(&builder), sameTypeResult, "");
        getPackageMutableRef().addStringOffsetRelocationEntry(lhsTypeName.data(), llvm::unwrap(lhsGep));

        LLVMValueRef castResult =
            LLVMBuildBitCast(llvm::wrap(&builder), dataLoad, lhsTypeRef, getLhsOperand().getName().c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(llvm::wrap(&builder), castResult, "");
        LLVMBuildStore(llvm::wrap(&builder), castLoad, lhsOpRef);
    } else if (Type::isSmartStructType(lhsTypeTag)) {
        getFunctionMutableRef().storeValueInSmartStruct(module, builder, llvm::unwrap(rhsOpRef), rhsType,
                                                        llvm::unwrap(lhsOpRef));
    } else {
        LLVMBuildBitCast(llvm::wrap(&builder), rhsOpRef, lhsTypeRef, "data_cast");
    }
}

LLVMValueRef TypeCastInsn::getIsSameTypeDeclaration(llvm::Module &module, LLVMValueRef lhsRef, LLVMValueRef rhsRef) {
    const std::string isSameTypeChar = "is_same_type";
    auto *functionRef = module.getFunction(isSameTypeChar);
    if (functionRef != nullptr) {
        return llvm::wrap(functionRef);
    }

    LLVMTypeRef paramTypes[] = {LLVMTypeOf(lhsRef), LLVMTypeOf(rhsRef)};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt8Type(), paramTypes, 2, 0);
    auto modRef = llvm::wrap(&module);
    return LLVMAddFunction(modRef, isSameTypeChar.c_str(), funcType);
}

} // namespace nballerina
