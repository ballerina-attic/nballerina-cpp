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

#ifndef __FUNCTIONCODEGEN__H__
#define __FUNCTIONCODEGEN__H__

#include "PackageCodeGen.h"
#include "interfaces/Translatable.h"
#include <map>
#include <string>

namespace nballerina {

class Operand;
class Function;

class FunctionCodeGen {
  private:
    PackageCodeGen &parentGenerator;
    std::map<std::string, llvm::BasicBlock *> basicBlocksMap;
    std::map<std::string, llvm::AllocaInst *> localVarRefs;

  public:
    FunctionCodeGen() = delete;
    FunctionCodeGen(PackageCodeGen &parentGenerator);
    ~FunctionCodeGen() = default;

    llvm::BasicBlock *getBasicBlock(const std::string &id);
    llvm::AllocaInst *getLocalVal(const std::string &varName) const;
    llvm::Value *getLocalOrGlobalVal(const Operand &op) const;
    llvm::Value *createTempVal(const Operand &op, llvm::IRBuilder<> &builder) const;
    static llvm::Type *getRetValType(const Function &obj, llvm::Module &module);

    void visit(class Function &obj, llvm::IRBuilder<> &builder);
};

} // namespace nballerina

#endif //!__FUNCTIONCODEGEN__H__
