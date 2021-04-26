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
#include "ConstantLoad.h"
#include "Function.h"
#include "NonTerminatorInsnCodeGen.h"
#include "Operand.h"
#include "Variable.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(ConstantLoadInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {
    assert(obj.getFunctionRef().getLocalOrGlobalVariable(obj.lhsOp).getType().getTypeTag() == obj.typeTag);
    llvm::Value *constRef = nullptr;
    switch (obj.typeTag) {
    case TYPE_TAG_INT: {
        constRef = builder.getInt64(std::get<int64_t>(obj.value));
        break;
    }
    case TYPE_TAG_FLOAT: {
        constRef = llvm::ConstantFP::get(module.getContext(), llvm::APFloat(std::get<double>(obj.value)));
        break;
    }
    case TYPE_TAG_BOOLEAN: {
        constRef = builder.getInt1(std::get<bool>(obj.value));
        break;
    }
    case TYPE_TAG_STRING:
    case TYPE_TAG_CHAR_STRING: {
        std::string stringValue = std::get<std::string>(obj.value);
        llvm::Constant *llvmConst = llvm::ConstantDataArray::getString(module.getContext(), stringValue);
        auto *globalStringValue = new llvm::GlobalVariable(module, llvmConst->getType(), false,
                                                           llvm::GlobalValue::PrivateLinkage, llvmConst, ".str");
        globalStringValue->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        globalStringValue->setAlignment(llvm::Align(1));
        auto *valueRef = builder.CreateInBoundsGEP(
            globalStringValue, llvm::ArrayRef<llvm::Value *>({builder.getInt64(0), builder.getInt64(0)}), "simple");
        auto addedStringRef = CodeGenUtils::getStringInitFunc(module);
        constRef = builder.CreateCall(
            addedStringRef, llvm::ArrayRef<llvm::Value *>({valueRef, builder.getInt64(stringValue.length())}));
        break;
    }
    case TYPE_TAG_NIL: {
        const auto &lhsOpName = obj.lhsOp.getName();
        const auto &funcRef = obj.getFunctionRef();
        if (funcRef.isMainFunction() && (lhsOpName == funcRef.getReturnVar()->getName())) {
            return;
        }
        constRef = builder.CreateLoad(CodeGenUtils::getGlobalNilVar(module), lhsOpName + "_temp");
        break;
    }
    default:
        llvm_unreachable("Invalid Type");
    }
    auto *lhsRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp, module);
    builder.CreateStore(constRef, lhsRef);
}

} // namespace nballerina
