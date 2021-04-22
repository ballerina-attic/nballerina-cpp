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

#include "PackageCodeGen.h"
#include "CodeGenUtils.h"
#include "FunctionCodeGen.h"
#include "FunctionParam.h"
#include "Package.h"
#include <iostream>

namespace nballerina {

void PackageCodeGen::visit(Package &obj, llvm::Module &module, llvm::IRBuilder<> &builder) {

    module.setSourceFileName(obj.sourceFileName);

    llvm::Type *charPtrType = builder.getInt8PtrTy();
    llvm::Constant *nullValue = llvm::Constant::getNullValue(charPtrType);

    // String Table initialization
    obj.strBuilder = std::make_unique<llvm::StringTableBuilder>(llvm::StringTableBuilder::RAW, 1);

    // creating external char pointer to store string builder table.
    obj.strBuilderGlobal = new llvm::GlobalVariable(module, charPtrType, false, llvm::GlobalValue::InternalLinkage,
                                                    nullValue, obj.STRING_TABLE_NAME, nullptr);
    obj.strBuilderGlobal->setAlignment(llvm::Align(4));

    // iterate over all global variables and translate
    for (auto const &it : obj.globalVars) {
        auto const &globVar = it.second;
        auto *varTyperef = CodeGenUtils::getLLVMTypeOfType(globVar.getType(), module);
        llvm::Constant *initValue = llvm::Constant::getNullValue(varTyperef);
        auto *gVar = new llvm::GlobalVariable(module, varTyperef, false, llvm::GlobalValue::ExternalLinkage, initValue,
                                              globVar.getName(), nullptr);
        gVar->setAlignment(llvm::Align(4));
    }

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    for (const auto &function : obj.functionLookUp) {
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
    for (auto &function : obj.functionLookUp) {
        if (function.second->isExternalFunction()) {
            continue;
        }
        FunctionCodeGen funcGenerator;
        funcGenerator.visit(*function.second, module, builder);
    }

    // This Api will finalize the string table builder if table size is not zero
    if (obj.strBuilder->getSize() != 0) {
        obj.applyStringOffsetRelocations(module, builder);
        // here, storing String builder table address into global char pointer.
        // like below example.
        // char arr[100] = { 'a' };
        // char *ptr = arr;
        auto *bitCastRes = builder.CreateBitCast(obj.strTablePtr, charPtrType, "");
        obj.strBuilderGlobal->setInitializer(llvm::dyn_cast<llvm::Constant>(bitCastRes));
    }
}

} // namespace nballerina
