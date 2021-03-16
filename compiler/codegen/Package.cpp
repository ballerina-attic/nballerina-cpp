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
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace nballerina {

// return ValueRef of global variable based on variable name.
LLVMValueRef Package::getGlobalLLVMVar(const std::string &globVar) const {
    const auto &varIt = globalVarRefs.find(globVar);
    if (varIt == globalVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

LLVMValueRef Package::getGlobalNilVar() const { return getGlobalLLVMVar(BAL_NIL_VALUE); }

const std::string &Package::getOrgName() const { return org; }
const std::string &Package::getPackageName() const { return name; }
const std::string &Package::getVersion() const { return version; }
const std::string &Package::getSrcFileName() const { return sourceFileName; }
LLVMValueRef Package::getStringBuilderTableGlobalPointer() { return strBuilderGlobalPtr; }

void Package::addToStrTable(std::string_view name) {
    if (!strBuilder->contains(name)) {
        strBuilder->add(name);
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

void Package::addFunctionRef(const std::string &arrayName, LLVMValueRef functionRef) {
    functionRefs.insert(std::pair<std::string, LLVMValueRef>(arrayName, functionRef));
}

LLVMTypeRef Package::getLLVMTypeOfType(const Type &type) const {
    switch (type.getTypeTag()) {
    case TYPE_TAG_INT:
        return LLVMInt32Type();
    case TYPE_TAG_BYTE:
    case TYPE_TAG_FLOAT:
        return LLVMFloatType();
    case TYPE_TAG_BOOLEAN:
        return LLVMInt8Type();
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING:
    case TYPE_TAG_MAP:
    case TYPE_TAG_ARRAY:
    case TYPE_TAG_NIL:
        return LLVMPointerType(LLVMInt8Type(), 0);
    case TYPE_TAG_ANY:
        return wrap(boxType.get());
    default:
        return LLVMInt32Type();
    }
}

void Package::translate(LLVMModuleRef &modRef) {

    // String Table initialization
    strBuilder = std::make_unique<llvm::StringTableBuilder>(llvm::StringTableBuilder::RAW, 1);

    // creating external char pointer to store string builder table.
    LLVMTypeRef charPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    llvm::Constant *charValue = llvm::Constant::getNullValue(llvm::unwrap(charPtrType));
    auto stringBuilderTable = std::make_unique<llvm::GlobalVariable>(*llvm::unwrap(modRef), llvm::unwrap(charPtrType),
                                                                     false, llvm::GlobalValue::InternalLinkage,
                                                                     charValue, STRING_TABLE_NAME, nullptr);
    stringBuilderTable->setAlignment(llvm::Align(4));
    strBuilderGlobalPtr = llvm::wrap(stringBuilderTable.get());
    globalStringValues.push_back(std::move(stringBuilderTable));

    // create global var for nil value
    llvm::Constant *nullValue = llvm::Constant::getNullValue(llvm::unwrap(LLVMPointerType(LLVMInt8Type(), 0)));
    auto gVal = std::make_unique<llvm::GlobalVariable>(
        *llvm::unwrap(modRef), llvm::unwrap(LLVMPointerType(LLVMInt8Type(), 0)), false,
        llvm::GlobalValue::InternalLinkage, nullValue, BAL_NIL_VALUE, nullptr);
    LLVMValueRef globVarRef = llvm::wrap(gVal.get());
    globalVarRefs.insert({BAL_NIL_VALUE, globVarRef});
    globalStringValues.push_back(std::move(gVal));

    // creating struct smart pointer to store any type variables data.
    LLVMTypeRef structGen = LLVMStructCreateNamed(LLVMGetGlobalContext(), "struct.smtPtr");
    LLVMTypeRef structElementTypes[] = {LLVMInt32Type(), LLVMInt32Type(), LLVMPointerType(LLVMInt8Type(), 0)};
    LLVMStructSetBody(structGen, structElementTypes, 3, 0);
    boxType = std::unique_ptr<llvm::StructType>(llvm::unwrap<llvm::StructType>(structGen));

    // iterate over all global variables and translate
    for (auto const &it : globalVars) {
        auto const &globVar = it.second;
        LLVMTypeRef varTyperef = getLLVMTypeOfType(globVar.getType());
        const std::string varName = globVar.getName();
        // emit/adding the global variable.
        llvm::Constant *initValue = llvm::Constant::getNullValue(llvm::unwrap(varTyperef));
        auto gVar =
            std::make_unique<llvm::GlobalVariable>(*llvm::unwrap(modRef), llvm::unwrap(varTyperef), false,
                                                   llvm::GlobalValue::ExternalLinkage, initValue, varName, nullptr);
        gVar->setAlignment(llvm::Align(4));
        LLVMValueRef globVarRef = wrap(gVar.get());
        globalVarRefs.insert({varName, globVarRef});
        globalStringValues.push_back(std::move(gVar));
    }

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    LLVMBuilderRef llvmBuilder = LLVMCreateBuilder();
    for (const auto &function : functionLookUp) {
        function.second->setLLVMBuilder(llvmBuilder);
        size_t numParams = function.second->getNumParams();
        std::unique_ptr<LLVMTypeRef[]> paramTypes(new LLVMTypeRef[numParams]);
        bool isVarArg = false;

        if (function.second->getRestParam()) {
            isVarArg = true;
        }
        assert(function.second->getReturnVar());
        size_t i = 0;
        for (const auto &funcParam : function.second->getParams()) {
            paramTypes[i++] = getLLVMTypeOfType(funcParam.getType());
        }

        LLVMTypeRef funcType = LLVMFunctionType(function.second->getLLVMTypeOfReturnVal(), paramTypes.get(), numParams,
                                                static_cast<LLVMBool>(isVarArg));
        if (funcType != nullptr) {
            function.second->setLLVMFunctionValue(
                LLVMAddFunction(modRef, function.second->getName().c_str(), funcType));
        }
    }

    // iterating over each function translate the function body
    for (auto &function : functionLookUp) {
        if (function.second->isExternalFunction()) {
            continue;
        }
        function.second->translate(modRef);
    }

    // This Api will finalize the string table builder if table size is not zero
    if (strBuilder->getSize() != 0) {
        applyStringOffsetRelocations(modRef);
        // here, storing String builder table address into global char pointer.
        // like below example.
        // char arr[100] = { 'a' };
        // char *ptr = arr;
        LLVMValueRef stringTableAddr = getGlobalLLVMVar(STRING_TABLE_NAME);
        LLVMValueRef stringTablePtrAddress = getStringBuilderTableGlobalPointer();
        LLVMValueRef bitCastRes =
            LLVMBuildBitCast(llvmBuilder, stringTableAddr, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMSetInitializer(stringTablePtrAddress, bitCastRes);
    }
}

void Package::addStringOffsetRelocationEntry(const std::string &eleType, LLVMValueRef storeInsn) {
    structElementStoreInst[eleType].push_back(storeInsn);
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void Package::applyStringOffsetRelocations(LLVMModuleRef &modRef) {

    strBuilder->finalizeInOrder();
    auto concatStringLen = strBuilder->getSize();
    std::unique_ptr<char[]> concatString(new char[concatStringLen]);
    auto *charPtr = concatString.get();
    for (const auto &element : structElementStoreInst) {
        const auto &eleType = element.first;
        charPtr = stpcpy(charPtr, eleType.c_str());
        charPtr++;
        size_t finalOrigOffset = strBuilder->getOffset(element.first);
        LLVMValueRef tempVal = LLVMConstInt(LLVMInt32Type(), finalOrigOffset, 0);
        for (const auto &insn : element.second) {
            LLVMValueRef constOperand;
            llvm::GetElementPtrInst *GEPInst = llvm::dyn_cast<llvm::GetElementPtrInst>(llvm::unwrap(insn));
            if (GEPInst)
                constOperand = LLVMGetOperand(insn, 1);
            else
                constOperand = LLVMGetOperand(insn, 0);
            LLVMReplaceAllUsesWith(constOperand, tempVal);
        }
    }
    LLVMTypeRef arraType = LLVMArrayType(LLVMInt8Type(), concatStringLen + 1);
    LLVMValueRef createAddrsSpace = LLVMAddGlobalInAddressSpace(modRef, arraType, STRING_TABLE_NAME.c_str(), 0);
    globalVarRefs.insert({STRING_TABLE_NAME, createAddrsSpace});

    LLVMValueRef constString = LLVMConstString(concatString.get(), concatStringLen, false);
    // Initializing global address space with generated string(concat all the
    // strings from string builder table).
    LLVMSetInitializer(createAddrsSpace, constString);
}

LLVMValueRef Package::getFunctionRef(const std::string &arrayName) const {
    const auto &it = functionRefs.find(arrayName);
    if (it == functionRefs.end()) {
        return nullptr;
    }
    return it->second;
}

std::optional<Variable> Package::getGlobalVariable(const std::string &name) const {
    const auto &varIt = globalVars.find(name);
    if (varIt == globalVars.end()) {
        return std::nullopt;
    }
    return varIt->second;
}

void Package::insertGlobalVar(Variable var) {
    globalVars.insert(std::pair<std::string, Variable>(var.getName(), std::move(var)));
}

} // namespace nballerina
