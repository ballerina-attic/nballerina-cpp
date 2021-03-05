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
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"

using namespace std;

namespace nballerina {

// return ValueRef of global variable based on variable name.
LLVMValueRef Package::getGlobalLLVMVar(const std::string &globVar) {
    auto varIt = globalVarRefs.find(globVar);
    if (varIt == globalVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

LLVMValueRef Package::getGlobalNilVar() { return getGlobalLLVMVar(BAL_NIL_VALUE); }

std::string Package::getOrgName() { return org; }
std::string Package::getPackageName() { return name; }
std::string Package::getVersion() { return version; }
std::string Package::getSrcFileName() { return sourceFileName; }
llvm::StringTableBuilder *Package::getStrTableBuilder() { return strBuilder; }
void Package::setOrgName(std::string orgName) { org = std::move(orgName); }
void Package::setPackageName(std::string pkgName) { name = std::move(pkgName); }
void Package::setVersion(std::string verName) { version = std::move(verName); }

void Package::setSrcFileName(std::string srcFileName) { sourceFileName = std::move(srcFileName); }

void Package::insertFunction(Function *function) {
    functions.push_back(function);
    functionLookUp.insert(std::pair<std::string, Function *>(function->getName(), function));
}

Function *Package::getFunction(const std::string &funcName) { return functionLookUp.at(funcName); }

void Package::addFunctionRef(const std::string &arrayName, LLVMValueRef functionRef) {
    functionRefs.insert(std::pair<std::string, LLVMValueRef>(arrayName, functionRef));
}

LLVMTypeRef Package::getLLVMTypeOfType(Type *typeD) {
    int typeTag = typeD->getTypeTag();
    switch (typeTag) {
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
        return LLVMPointerType(LLVMInt8Type(), 0);
    case TYPE_TAG_NIL:
        return LLVMPointerType(LLVMInt8Type(), 0);
    case TYPE_TAG_ANY:
        return wrap(boxType);
    default:
        return LLVMInt32Type();
    }
}

void Package::translate(LLVMModuleRef &modRef) {

    // String Table initialization
    strBuilder = new llvm::StringTableBuilder(llvm::StringTableBuilder::RAW, 1);

    // iterate over all global variables and translate
    for (auto const &it : globalVars) {
        Variable *globVar = it.second;
        LLVMTypeRef varTyperef = getLLVMTypeOfType(globVar->getTypeDecl());
        string varName = globVar->getName();
        // emit/adding the global variable.
        llvm::Constant *initValue = llvm::Constant::getNullValue(llvm::unwrap(varTyperef));
        llvm::GlobalVariable *gVar =
            new llvm::GlobalVariable(*llvm::unwrap(modRef), llvm::unwrap(varTyperef), false,
                                     llvm::GlobalValue::ExternalLinkage, initValue, varName, nullptr);
        gVar->setAlignment(llvm::Align(4));
        LLVMValueRef globVarRef = wrap(gVar);
        globalVarRefs.insert({varName, globVarRef});
    }

    // create global var for nil value
    llvm::Constant *nullValue = llvm::Constant::getNullValue(llvm::unwrap(LLVMPointerType(LLVMInt8Type(), 0)));
    llvm::GlobalVariable *gVar =
        new llvm::GlobalVariable(*llvm::unwrap(modRef), llvm::unwrap(LLVMPointerType(LLVMInt8Type(), 0)), false,
                                 llvm::GlobalValue::InternalLinkage, nullValue, BAL_NIL_VALUE, 0);
    LLVMValueRef globVarRef = llvm::wrap(gVar);
    globalVarRefs.insert({BAL_NIL_VALUE, globVarRef});

    // creating struct smart pointer to store any type variables data.
    LLVMTypeRef structGen = LLVMStructCreateNamed(LLVMGetGlobalContext(), "struct.smtPtr");
    LLVMTypeRef *structElementTypes = new LLVMTypeRef[3];
    structElementTypes[0] = LLVMInt32Type();
    structElementTypes[1] = LLVMInt32Type();
    structElementTypes[2] = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMStructSetBody(structGen, structElementTypes, 3, 0);
    boxType = llvm::unwrap<llvm::StructType>(structGen);

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    for (const auto &function : functions) {
        function->setLLVMBuilder(LLVMCreateBuilder());
        size_t numParams = function->getNumParams();
        LLVMTypeRef *paramTypes = new LLVMTypeRef[numParams];
        bool isVarArg = false;

        if (function->getRestParam() != nullptr) {
            isVarArg = true;
        }
        assert(function->getReturnVar());
        for (size_t i = 0; i < numParams; i++) {
            FunctionParam *funcParam = function->getParam(i);
            assert(funcParam->getType());
            paramTypes[i] = getLLVMTypeOfType(funcParam->getType());
        }

        LLVMTypeRef funcType = LLVMFunctionType(function->getLLVMTypeOfReturnVal(), paramTypes, numParams,
                                                static_cast<LLVMBool>(isVarArg));
        if (funcType != nullptr) {
            function->setLLVMFunctionValue(LLVMAddFunction(modRef, function->getName().c_str(), funcType));
        }
    }

    // iterating over each function translate the function body.
    for (const auto &function : functions) {
        if (function->isExternalFunction())
            continue;
        function->translate(modRef);
    }

    // This Api will finalize the string table builder if table size is not
    // zero.
    if (strBuilder->getSize() != 0) {
        applyStringOffsetRelocations();
    }
}

void Package::addStringOffsetRelocationEntry(const std::string &eleType, LLVMValueRef storeInsn) {
    structElementStoreInst[eleType].push_back(storeInsn);
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void Package::applyStringOffsetRelocations() {

    strBuilder->finalize();
    for (const auto &element : structElementStoreInst) {
        size_t finalOrigOffset = strBuilder->getOffset(element.first);
        LLVMValueRef tempVal = LLVMConstInt(LLVMInt32Type(), finalOrigOffset, 0);
        for (const auto &insn : element.second) {
            LLVMValueRef constOperand = LLVMGetOperand(insn, 0);
            LLVMReplaceAllUsesWith(constOperand, tempVal);
            break;
        }
    }
}

LLVMValueRef Package::getFunctionRef(const std::string &arrayName) {
    auto it = functionRefs.find(arrayName);
    if (it == functionRefs.end()) {
        return nullptr;
    }
    return it->second;
}

Variable *Package::getGlobalVariable(const std::string &name) {
    auto varIt = globalVars.find(name);
    if (varIt == globalVars.end()) {
        return nullptr;
    }
    return varIt->second;
}

void Package::insertGlobalVar(Variable *var) {
    globalVars.insert(std::pair<std::string, Variable *>(var->getName(), var));
}

} // namespace nballerina
