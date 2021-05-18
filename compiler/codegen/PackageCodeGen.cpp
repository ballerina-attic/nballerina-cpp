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

#include "codegen/PackageCodeGen.h"
#include "bir/FunctionParam.h"
#include "bir/Package.h"
#include "codegen/CodeGenUtils.h"
#include "codegen/FunctionCodeGen.h"
#include <llvm/IR/Verifier.h>

namespace nballerina {

PackageCodeGen::PackageCodeGen(llvm::Module &module)
    : module(module), globalStrTable(nullptr), globalStrTable2(nullptr) {}

llvm::Module &PackageCodeGen::getModule() { return module; }

void PackageCodeGen::visit(Package &obj, llvm::IRBuilder<> &builder) {

    module.setSourceFileName(obj.sourceFileName);

    llvm::Type *charPtrType = builder.getInt8PtrTy();
    llvm::Constant *nullValue = llvm::Constant::getNullValue(charPtrType);

    // String Table initialization
    strBuilder = std::make_unique<llvm::StringTableBuilder>(llvm::StringTableBuilder::RAW, 1);

    // creating external char pointer to store string builder table.
    globalStrTable = new llvm::GlobalVariable(module, charPtrType, false, llvm::GlobalValue::InternalLinkage, nullValue,
                                              STRING_TABLE_NAME, nullptr);
    globalStrTable->setAlignment(llvm::Align(4));

    // iterate over all global variables and translate
    for (auto const &globVar : obj.globalVars) {
        auto *varTyperef = CodeGenUtils::getLLVMTypeOfType(globVar.getType(), module);
        llvm::Constant *initValue = llvm::Constant::getNullValue(varTyperef);
        auto *gVar = new llvm::GlobalVariable(module, varTyperef, false, llvm::GlobalValue::ExternalLinkage, initValue,
                                              globVar.getName(), nullptr);
        gVar->setAlignment(llvm::Align(4));
    }

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    for (const auto &function : obj.functions) {
        auto numParams = function.getNumParams();
        std::vector<llvm::Type *> paramTypes;
        paramTypes.reserve(numParams);
        for (const auto &funcParam : function.getParams()) {
            paramTypes.push_back(CodeGenUtils::getLLVMTypeOfType(funcParam.getType(), module));
        }

        bool isVarArg = static_cast<bool>(function.getRestParam());
        auto *funcType =
            llvm::FunctionType::get(FunctionCodeGen::getRetValType(function, module), paramTypes, isVarArg);

        llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, function.getName(), module);
    }

    // iterating over each function translate the function body
    for (auto &function : obj.functions) {
        if (function.isExternalFunction()) {
            continue;
        }
        FunctionCodeGen funcGenerator(*this);
        funcGenerator.visit(function, builder);
    }

    // This Api will finalize the string table builder if table size is not zero
    if (strBuilder->getSize() != 0) {
        applyStringOffsetRelocations(builder);
        // here, storing String builder table address into global char pointer.
        // like below example.
        // char arr[100] = { 'a' };
        // char *ptr = arr;
        auto *bitCastRes = builder.CreateBitCast(globalStrTable2, charPtrType, "");
        globalStrTable->setInitializer(llvm::dyn_cast<llvm::Constant>(bitCastRes));
    }

    assert(!llvm::verifyModule(module, &llvm::outs()));
}

llvm::Value *PackageCodeGen::addToStringTable(std::string_view newString, llvm::IRBuilder<> &builder) {
    if (!strBuilder->contains(newString.data())) {
        strBuilder->add(newString.data());
    }
    int tempRandNum1 = std::rand() % 1000 + 1;
    auto *constValue = builder.getInt64(tempRandNum1);
    auto *strTblLoad = builder.CreateLoad(globalStrTable);
    auto *strTablePosition = builder.CreateInBoundsGEP(strTblLoad, llvm::ArrayRef<llvm::Value *>({constValue}));
    structElementStoreInst[newString.data()].push_back(strTablePosition);
    return strTablePosition;
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void PackageCodeGen::applyStringOffsetRelocations(llvm::IRBuilder<> &builder) {

    // finalizing the string builder table.
    strBuilder->finalize();
    // After finalize the string table, re arranging the actual offset values.
    std::vector<std::pair<size_t, std::string>> offsetStringPair;
    offsetStringPair.reserve(structElementStoreInst.size());

    for (const auto &element : structElementStoreInst) {
        const std::string &typeString = element.first;
        size_t finalOrigOffset = strBuilder->getOffset(element.first);
        offsetStringPair.emplace_back(finalOrigOffset, typeString);
    }

    // creating the concat string to store in the global address space(string table
    // global pointer)
    std::string concatString;
    std::sort(offsetStringPair.begin(), offsetStringPair.end());
    for (const auto &pair : offsetStringPair) {
        concatString.append(pair.second);
    }

    for (const auto &element : structElementStoreInst) {
        size_t finalOrigOffset = strBuilder->getOffset(element.first);
        auto *tempVal = builder.getInt64(finalOrigOffset);
        for (const auto &insn : element.second) {
            auto *GEPInst = llvm::dyn_cast<llvm::GetElementPtrInst>(insn);
            if (GEPInst != nullptr) {
                GEPInst->getOperand(1)->replaceAllUsesWith(tempVal);
                continue;
            }
            auto *temp = llvm::dyn_cast<llvm::User>(insn);
            if (temp != nullptr) {
                temp->getOperand(0)->replaceAllUsesWith(tempVal);
            } else {
                llvm_unreachable("");
            }
        }
    }

    auto *arrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(module.getContext()), concatString.size() + 1);
    globalStrTable2 = new llvm::GlobalVariable(module, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr,
                                               STRING_TABLE_NAME, nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
    auto *constString = llvm::ConstantDataArray::getString(module.getContext(), concatString);
    // Initializing global address space with generated string(concat all the
    // strings from string builder table).
    globalStrTable2->setInitializer(constString);
}

} // namespace nballerina
