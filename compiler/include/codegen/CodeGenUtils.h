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

#ifndef __CODEGENUTILS__H__
#define __CODEGENUTILS__H__

#include "bir/Types.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>

namespace nballerina {

class CodeGenUtils {
  private:
    CodeGenUtils() = default;
    static llvm::Function *createIntToAnyFunction(llvm::Module &module);
    static llvm::Function *createAnyToIntFunction(llvm::Module &module);
    static llvm::Function *getIntToAnyFunction(llvm::Module &module);

  public:
    ~CodeGenUtils() = default;

    static llvm::Type *getLLVMTypeOfType(const Type &type, llvm::Module &module);
    static llvm::Type *getLLVMTypeOfType(TypeTag typeTag, llvm::Module &module);
    static llvm::Value *getGlobalNilVar(llvm::Module &module);
    static llvm::FunctionCallee getAbortFunc(llvm::Module &module);
    static llvm::FunctionCallee getMapSpreadFieldInitFunc(llvm::Module &module);
    static llvm::FunctionCallee getStringInitFunc(llvm::Module &module);
    static llvm::FunctionCallee getArrayStoreFunc(llvm::Module &module, TypeTag memberTypeTag);
    static llvm::FunctionCallee getArrayInitFunc(llvm::Module &module, TypeTag memberTypeTag);
    static llvm::FunctionCallee getArrayLoadFunc(llvm::Module &module, TypeTag memberTypeTag);
    static llvm::FunctionCallee getBoxValueFunc(llvm::Module &module, llvm::Type *paramType, TypeTag typeTag);
    static llvm::FunctionCallee getIsSameTypeFunc(llvm::Module &module, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Function *getAnyToIntFunction(llvm::Module &module);
    static llvm::Value *createBalValue(llvm::Module &module, llvm::IRBuilder<> &builder, llvm::Value *value,
                                       const Type &valueType);
    static llvm::FunctionCallee getNewMapInitFunc(llvm::Module &module);
    static llvm::FunctionCallee getMapLoadFunc(llvm::Module &module);
    static llvm::FunctionCallee getMapStoreFunc(llvm::Module &module);
};

} // namespace nballerina

#endif //!__CODEGENUTILS__H__
