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

#include "bir/ConstantLoad.h"
#include "bir/Function.h"
#include "bir/Operand.h"
#include "bir/Variable.h"
#include "codegen/CodeGenUtils.h"
#include "codegen/NonTerminatorInsnCodeGen.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(ConstantLoadInsn &obj, llvm::IRBuilder<> &builder) {
    assert(obj.getFunctionRef().getLocalOrGlobalVariable(obj.lhsOp).getType().getTypeTag() == obj.typeTag);
    llvm::Value *constRef = nullptr;
    switch (obj.typeTag) {
    case TYPE_TAG_INT: {
        constRef = builder.getInt64(std::get<int64_t>(obj.value));
        break;
    }
    case TYPE_TAG_FLOAT: {
        constRef =
            llvm::ConstantFP::get(moduleGenerator.getModule().getContext(), llvm::APFloat(std::get<double>(obj.value)));
        break;
    }
    case TYPE_TAG_BOOLEAN: {
        constRef = builder.getInt1(std::get<bool>(obj.value));
        break;
    }
    case TYPE_TAG_STRING:
    case TYPE_TAG_CHAR_STRING: {
        std::string stringValue = std::get<std::string>(obj.value);
        // Header, Length and String
        const unsigned int HEADER = 0b000110;
        auto *header = llvm::ConstantInt::get(builder.getInt64Ty(), HEADER, false);
        auto *size = llvm::ConstantInt::get(builder.getInt64Ty(), stringValue.length(), false);
        auto *string = llvm::ConstantDataArray::getString(moduleGenerator.getModule().getContext(), stringValue, false);

        // Create global constant of string
        auto *globalValue = new llvm::GlobalVariable(moduleGenerator.getModule(), string->getType(), true,
                                                     llvm::GlobalValue::PrivateLinkage, string, ".str");
        auto *valueRef = builder.CreateInBoundsGEP(
            globalValue, llvm::ArrayRef<llvm::Value *>({builder.getInt64(0), builder.getInt64(0)}), "simple");

        // Create constant elements initializer of balAsciiString members
        llvm::ArrayRef<llvm::Constant *> elements = {header, size, static_cast<llvm::Constant *>(valueRef)};
        auto *structType = static_cast<llvm::StructType *>(
            CodeGenUtils::getLLVMTypeOfTypeStruct(obj.typeTag, moduleGenerator.getModule()));

        // Create constant struct object of balAsciiString
        auto *constStruct = llvm::ConstantStruct::get(structType, elements);
        auto *globalStruct = new llvm::GlobalVariable(moduleGenerator.getModule(), structType, true,
                                                      llvm::GlobalValue::PrivateLinkage, constStruct, "bal_string");
        auto *ptr = new llvm::GlobalVariable(moduleGenerator.getModule(), llvm::PointerType::get(structType, 0), true,
                                             llvm::GlobalValue::PrivateLinkage, globalStruct, "ptr");
        constRef = builder.CreateLoad(ptr);
        break;
    }
    case TYPE_TAG_NIL: {
        const auto &lhsOpName = obj.lhsOp.getName();
        const auto &funcRef = obj.getFunctionRef();
        if (funcRef.isMainFunction() && (lhsOpName == funcRef.getReturnVar()->getName())) {
            return;
        }
        constRef = builder.CreateLoad(CodeGenUtils::getGlobalNilVar(moduleGenerator.getModule()), lhsOpName + "_temp");
        break;
    }
    default:
        llvm_unreachable("Invalid Type");
    }
    auto *lhsRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    builder.CreateStore(constRef, lhsRef);
}

} // namespace nballerina
