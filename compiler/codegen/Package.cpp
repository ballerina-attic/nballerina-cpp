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
#include "Function.h"
#include "FunctionParam.h"
#include "Operand.h"
#include "Types.h"

namespace nballerina {

const llvm::Value *Package::getGlobalNilVar() const { return nillGlobal; }

const std::string &Package::getOrgName() const { return org; }
const std::string &Package::getPackageName() const { return name; }
const std::string &Package::getVersion() const { return version; }
const std::string &Package::getSrcFileName() const { return sourceFileName; }
const llvm::Value *Package::getStringBuilderTableGlobalPointer() const { return strBuilderGlobal; }

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

llvm::Type *Package::getLLVMTypeOfType(const Type &type, llvm::Module &module) const {
    return getLLVMTypeOfType(type.getTypeTag(), module);
}

llvm::Type *Package::getLLVMTypeOfType(TypeTag typeTag, llvm::Module &module) const {
    auto &context = module.getContext();
    switch (typeTag) {
    case TYPE_TAG_INT:
        return llvm::Type::getInt64Ty(context);
    case TYPE_TAG_FLOAT:
        return llvm::Type::getFloatTy(context);
    case TYPE_TAG_BOOLEAN:
        return llvm::Type::getInt8Ty(context);
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING:
    case TYPE_TAG_MAP:
    case TYPE_TAG_ARRAY:
    case TYPE_TAG_NIL:
        return llvm::Type::getInt8PtrTy(context);
    case TYPE_TAG_ANY:
    case TYPE_TAG_UNION:
    case TYPE_TAG_ANYDATA:
        return boxType;
    default:
        return llvm::Type::getInt64Ty(context);
    }
}

void Package::translate(llvm::Module &module) {

    auto &context = module.getContext();
    llvm::Type *charPtrType = llvm::Type::getInt8PtrTy(context);
    llvm::Constant *nullValue = llvm::Constant::getNullValue(charPtrType);

    // String Table initialization
    strBuilder = std::make_unique<llvm::StringTableBuilder>(llvm::StringTableBuilder::RAW, 1);

    // creating external char pointer to store string builder table.
    strBuilderGlobal = new llvm::GlobalVariable(module, charPtrType, false, llvm::GlobalValue::InternalLinkage,
                                                nullValue, STRING_TABLE_NAME, nullptr);
    strBuilderGlobal->setAlignment(llvm::Align(4));

    // create global var for nil value
    nillGlobal = new llvm::GlobalVariable(module, charPtrType, false, llvm::GlobalValue::InternalLinkage, nullValue,
                                          BAL_NIL_VALUE, nullptr);

    // creating struct smart pointer to store any type variables data.
    boxType = llvm::StructType::create(
        context, llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(context), llvm::Type::getInt8PtrTy(context)}),
        "struct.smtPtr");

    // iterate over all global variables and translate
    for (auto const &it : globalVars) {
        auto const &globVar = it.second;
        auto *varTyperef = getLLVMTypeOfType(globVar.getType(), module);
        llvm::Constant *initValue = llvm::Constant::getNullValue(varTyperef);
        auto *gVar = new llvm::GlobalVariable(module, varTyperef, false, llvm::GlobalValue::ExternalLinkage, initValue,
                                              globVar.getName(), nullptr);
        gVar->setAlignment(llvm::Align(4));
    }

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    auto builder = std::make_shared<llvm::IRBuilder<>>(context);
    for (const auto &function : functionLookUp) {
        function.second->setLLVMBuilder(builder.get());
        auto numParams = function.second->getNumParams();
        std::vector<llvm::Type *> paramTypes;
        paramTypes.reserve(numParams);
        for (const auto &funcParam : function.second->getParams()) {
            paramTypes.push_back(getLLVMTypeOfType(funcParam.getType(), module));
        }

        bool isVarArg = function.second->getRestParam() ? true : false;
        auto *funcType = llvm::FunctionType::get(function.second->getLLVMTypeOfReturnVal(module), paramTypes, isVarArg);

        llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, function.second->getName(), module);
    }

    // iterating over each function translate the function body
    for (auto &function : functionLookUp) {
        if (function.second->isExternalFunction()) {
            continue;
        }
        function.second->translate(module);
    }

    // This Api will finalize the string table builder if table size is not zero
    if (strBuilder->getSize() != 0) {
        applyStringOffsetRelocations(module);
        // here, storing String builder table address into global char pointer.
        // like below example.
        // char arr[100] = { 'a' };
        // char *ptr = arr;
        auto *bitCastRes = builder->CreateBitCast(strTablePtr, charPtrType, "");
        strBuilderGlobal->setInitializer(llvm::dyn_cast<llvm::Constant>(bitCastRes));
    }
}

void Package::addStringOffsetRelocationEntry(const std::string &eleType, llvm::Value *storeInsn) {
    structElementStoreInst[eleType].push_back(storeInsn);
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void Package::applyStringOffsetRelocations(llvm::Module &module) {

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
        auto *tempVal = llvm::ConstantInt::get(module.getContext(), llvm::APInt(64, finalOrigOffset, false));
        for (const auto &insn : element.second) {
            auto *GEPInst = llvm::dyn_cast<llvm::GetElementPtrInst>(insn);
            auto *userVal = llvm::dyn_cast<llvm::User>(insn);
            llvm::Value *val = (GEPInst != nullptr) ? userVal->getOperand(1) : userVal->getOperand(0);
            val->replaceAllUsesWith(tempVal);
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

// Declaration for map<int> type store function
llvm::FunctionCallee Package::getMapStoreDeclaration(llvm::Module &module, TypeTag typeTag) const {
    const std::string funcName = "map_store_" + Type::getNameOfType(typeTag);
    llvm::Type *memberType = Type::isSmartStructType(typeTag)
                                 ? llvm::PointerType::get(getLLVMTypeOfType(typeTag, module), 0)
                                 : getLLVMTypeOfType(typeTag, module);
    auto *keyType = getLLVMTypeOfType(TYPE_TAG_STRING, module);
    auto *mapType = getLLVMTypeOfType(TYPE_TAG_MAP, module);
    auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                             llvm::ArrayRef<llvm::Type *>({mapType, keyType, memberType}), false);
    return module.getOrInsertFunction(funcName, funcType);
}

} // namespace nballerina
