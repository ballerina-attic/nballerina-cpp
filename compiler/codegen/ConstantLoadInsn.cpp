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

#include "ConstantLoad.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace nballerina {

// With Nil Type setting only Type Tag because value will be zero with NIL Type.
ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, BasicBlock &currentBB)
    : NonTerminatorInsn(lhs, currentBB), typeTag(TYPE_TAG_NIL) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, BasicBlock &currentBB, int64_t intVal)
    : NonTerminatorInsn(lhs, currentBB), typeTag(TYPE_TAG_INT), value(intVal) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, BasicBlock &currentBB, float floatVal)
    : NonTerminatorInsn(lhs, currentBB), typeTag(TYPE_TAG_FLOAT), value(floatVal) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, BasicBlock &currentBB, bool boolVal)
    : NonTerminatorInsn(lhs, currentBB), typeTag(TYPE_TAG_BOOLEAN), value(boolVal) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, BasicBlock &currentBB, std::string str)
    : NonTerminatorInsn(lhs, currentBB), typeTag(TYPE_TAG_STRING), value(std::move(str)) {}

llvm::FunctionCallee ConstantLoadInsn::getNewString(llvm::Module &module) {
    const std::string newString = "new_string";
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt64Ty(module.getContext())}),
                                false);
    return module.getOrInsertFunction(newString, funcType);
}

void ConstantLoadInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    LLVMValueRef constRef = nullptr;
    const auto &lhsOp = getLhsOperand();
    const auto &funcRef = getFunctionRef();
    LLVMValueRef lhsRef = llvm::wrap(funcRef.getLLVMLocalOrGlobalVar(lhsOp, module));

    assert(funcRef.getLocalOrGlobalVariable(lhsOp).getType().getTypeTag() == typeTag);

    switch (typeTag) {
    case TYPE_TAG_INT: {
        constRef = LLVMConstInt(LLVMInt64Type(), std::get<int64_t>(value), 0);
        break;
    }
    case TYPE_TAG_FLOAT: {
        constRef = LLVMConstReal(LLVMFloatType(), std::get<float>(value));
        break;
    }
    case TYPE_TAG_BOOLEAN: {
        if (std::get<bool>(value)) {
            constRef = LLVMConstInt(LLVMInt8Type(), 1, 0);
        } else {
            constRef = LLVMConstInt(LLVMInt8Type(), 0, 0);
        }
        break;
    }
    case TYPE_TAG_STRING:
    case TYPE_TAG_CHAR_STRING: {
        std::string stringValue = std::get<std::string>(value);
        llvm::Constant *C = llvm::ConstantDataArray::getString(
            module.getContext(), llvm::StringRef(stringValue.c_str(), stringValue.length()));
        auto *globalStringValue =
            new llvm::GlobalVariable(module, C->getType(), false, llvm::GlobalValue::PrivateLinkage, C, ".str");
        globalStringValue->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        globalStringValue->setAlignment(llvm::Align(1));

        LLVMValueRef paramTypes[] = {llvm::wrap(builder.getInt64(0)), llvm::wrap(builder.getInt64(0))};
        LLVMValueRef valueRef =
            LLVMBuildInBoundsGEP(llvm::wrap(&builder), wrap(globalStringValue), paramTypes, 2, "simple");
        LLVMValueRef addedStringRef = llvm::wrap(getNewString(module).getCallee());

        LLVMValueRef sizeOpValueRef[] = {valueRef, LLVMConstInt(LLVMInt64Type(), stringValue.length(), 0)};
        constRef = LLVMBuildCall(llvm::wrap(&builder), addedStringRef, sizeOpValueRef, 2, "");
        break;
    }
    case TYPE_TAG_NIL: {
        const auto &lhsOpName = lhsOp.getName();
        // check for the main function and () is assigned to 0%
        assert(funcRef.getReturnVar().has_value());
        if (funcRef.isMainFunction() && (lhsOpName == funcRef.getReturnVar()->getName())) {
            return;
        }
        const auto *constTempRef = getPackageRef().getGlobalNilVar();
        std::string tempName = lhsOpName + "_temp";
        constRef = LLVMBuildLoad(llvm::wrap(&builder), llvm::wrap(constTempRef), tempName.c_str());
        break;
    }
    default:
        llvm_unreachable("Unknown Type");
    }

    if ((constRef != nullptr) && (lhsRef != nullptr)) {
        LLVMBuildStore(llvm::wrap(&builder), constRef, lhsRef);
    }
}

} // namespace nballerina
