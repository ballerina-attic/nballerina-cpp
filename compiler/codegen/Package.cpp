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

#include "Package.h"
#include "CodeGenUtils.h"
#include "Function.h"
#include "FunctionParam.h"
#include "Operand.h"
#include "Types.h"

namespace nballerina {

std::string Package::getModuleName() const { return org + name + version; }
llvm::Value *Package::getStringBuilderTableGlobalPointer() const { return strBuilderGlobal; }

void Package::addToStrTable(std::string_view name) {
    if (!strBuilder->contains(name.data())) {
        strBuilder->add(name.data());
    }
}
void Package::setOrgName(std::string orgName) { org = std::move(orgName); }
void Package::setPackageName(std::string pkgName) { name = std::move(pkgName); }
void Package::setVersion(std::string verName) { version = std::move(verName); }

void Package::setSrcFileName(std::string srcFileName) { sourceFileName = std::move(srcFileName); }

void Package::insertFunction(const std::shared_ptr<Function> &function) {
    functionLookUp.insert(std::pair<std::string, std::shared_ptr<Function>>(function->getName(), function));
}

const Function &Package::getFunction(const std::string &name) const { return *functionLookUp.at(name); }

void Package::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {

    module.setSourceFileName(sourceFileName);

    llvm::Type *charPtrType = builder.getInt8PtrTy();
    llvm::Constant *nullValue = llvm::Constant::getNullValue(charPtrType);

    // String Table initialization
    strBuilder = std::make_unique<llvm::StringTableBuilder>(llvm::StringTableBuilder::RAW, 1);

    // creating external char pointer to store string builder table.
    strBuilderGlobal = new llvm::GlobalVariable(module, charPtrType, false, llvm::GlobalValue::InternalLinkage,
                                                nullValue, STRING_TABLE_NAME, nullptr);
    strBuilderGlobal->setAlignment(llvm::Align(4));

    // iterate over all global variables and translate
    for (auto const &it : globalVars) {
        auto const &globVar = it.second;
        auto *varTyperef = CodeGenUtils::getLLVMTypeOfType(globVar.getType(), module);
        llvm::Constant *initValue = llvm::Constant::getNullValue(varTyperef);
        auto *gVar = new llvm::GlobalVariable(module, varTyperef, false, llvm::GlobalValue::ExternalLinkage, initValue,
                                              globVar.getName(), nullptr);
        gVar->setAlignment(llvm::Align(4));
    }

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    for (const auto &function : functionLookUp) {
        auto numParams = function.second->getNumParams();
        std::vector<llvm::Type *> paramTypes;
        paramTypes.reserve(numParams);
        for (const auto &funcParam : function.second->getParams()) {
            paramTypes.push_back(CodeGenUtils::getLLVMTypeOfType(funcParam.getType(), module));
        }

        bool isVarArg = static_cast<bool>(function.second->getRestParam());
        auto *funcType = llvm::FunctionType::get(function.second->getLLVMTypeOfReturnVal(module), paramTypes, isVarArg);

        llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, function.second->getName(), module);
    }

    // iterating over each function translate the function body
    for (auto &function : functionLookUp) {
        if (function.second->isExternalFunction()) {
            continue;
        }
        function.second->translate(module, builder);
    }

    // This Api will finalize the string table builder if table size is not zero
    if (strBuilder->getSize() != 0) {
        applyStringOffsetRelocations(module, builder);
        // here, storing String builder table address into global char pointer.
        // like below example.
        // char arr[100] = { 'a' };
        // char *ptr = arr;
        auto *bitCastRes = builder.CreateBitCast(strTablePtr, charPtrType, "");
        strBuilderGlobal->setInitializer(llvm::dyn_cast<llvm::Constant>(bitCastRes));
    }
}

void Package::addStringOffsetRelocationEntry(const std::string &eleType, llvm::Value *storeInsn) {
    structElementStoreInst[eleType].push_back(storeInsn);
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void Package::applyStringOffsetRelocations(llvm::Module &module, llvm::IRBuilder<> &builder) {

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
    strTablePtr = new llvm::GlobalVariable(module, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr,
                                           STRING_TABLE_NAME, nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
    auto *constString = llvm::ConstantDataArray::getString(module.getContext(), concatString);
    // Initializing global address space with generated string(concat all the
    // strings from string builder table).
    strTablePtr->setInitializer(constString);
}

const Variable &Package::getGlobalVariable(const std::string &name) const {
    const auto &varIt = globalVars.find(name);
    assert(varIt != globalVars.end());
    return varIt->second;
}

void Package::insertGlobalVar(const Variable &var) {
    globalVars.insert(std::pair<std::string, Variable>(var.getName(), var));
}

void Package::storeValueInSmartStruct(llvm::Module &module, llvm::IRBuilder<> &builder, llvm::Value *value,
                                      const Type &valueType, llvm::Value *smartStruct) {

    // struct first element original type
    auto *inherentTypeIdx = builder.CreateStructGEP(smartStruct, 0, "inherentTypeName");
    auto valueTypeName = Type::typeStringMangleName(valueType);
    addToStrTable(valueTypeName);
    int tempRandNum1 = std::rand() % 1000 + 1;
    auto *constValue = builder.getInt64(tempRandNum1);
    auto *storeInsn = builder.CreateStore(constValue, inherentTypeIdx);
    addStringOffsetRelocationEntry(valueTypeName.data(), storeInsn);

    // struct second element void pointer data.
    auto *valueIndx = builder.CreateStructGEP(smartStruct, 1, "data");
    if (Type::isBoxValueSupport(valueType.getTypeTag())) {
        auto *valueTemp = builder.CreateLoad(value, "_temp");
        auto boxValFunc = CodeGenUtils::getBoxValueFunc(module, valueTemp->getType(), valueType.getTypeTag());
        value = builder.CreateCall(boxValFunc, llvm::ArrayRef<llvm::Value *>({valueTemp}), "call");
    }
    auto *bitCastRes = builder.CreateBitCast(value, builder.getInt8PtrTy(), "");
    builder.CreateStore(bitCastRes, valueIndx);
}

void Package::storeValueInBalAsciiString(llvm::IRBuilder<> &builder, llvm::Value *value, std::string stringValue,
                                         llvm::Value *balString) {
    // struct first element - header
    auto *header = builder.CreateStructGEP(balString, 0, "header");
    auto *constValue = builder.getInt64(0b000110);
    builder.CreateStore(constValue, header);

    // struct second element - string length
    auto *len = builder.CreateStructGEP(balString, 1, "length");
    auto *lenValue = builder.getInt64(stringValue.length());
    builder.CreateStore(lenValue, len);

    // struct third element - char array data
    auto *utf = builder.CreateStructGEP(balString, 2, "data");
    auto *loadUtf = builder.CreateLoad(utf, "_temp");
    builder.CreateMemCpy(loadUtf, llvm::Align(1), value, llvm::Align(1), stringValue.length());
}

} // namespace nballerina
