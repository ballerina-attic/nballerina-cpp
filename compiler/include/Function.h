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

#ifndef __FUNCTION__H__
#define __FUNCTION__H__

#include "RestParam.h"
#include "Variable.h"
#include "interfaces/Debuggable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;
class FunctionParam;
class InvocableType;
class Type;
class Package;

class Function : public Debuggable {
  private:
    inline static const std::string MAIN_FUNCTION_NAME = "main";
    static constexpr unsigned int PUBLIC = 1;
    static constexpr unsigned int NATIVE = PUBLIC << 1;
    Package &parentPackage;
    std::string name;
    std::string workerName;
    unsigned int flags;
    std::optional<Variable> returnVar;
    std::optional<RestParam> restParam;
    bool isBBMapEmpty = true;
    std::weak_ptr<BasicBlock> firstBlock;
    std::map<std::string, Variable> localVars;
    std::map<std::string, std::shared_ptr<BasicBlock>> basicBlocksMap;
    std::vector<FunctionParam> requiredParams;
    llvm::IRBuilder<> *llvmBuilder;
    std::map<std::string, llvm::Value *> branchComparisonList;
    std::map<std::string, llvm::AllocaInst *> localVarRefs;
    std::weak_ptr<BasicBlock> FindBasicBlock(const std::string &id);
    llvm::FunctionCallee generateAbortInsn(llvm::Module &module);
    void splitBBIfPossible(llvm::Module &module);
    static bool isBoxValueSupport(TypeTag typeTag);
    llvm::FunctionCallee generateBoxValueFunc(llvm::Module &module, llvm::Type *paramType, TypeTag typeTag);

  public:
    Function() = delete;
    Function(Package &parentPackage, std::string name, std::string workerName, unsigned int flags);
    Function(const Function &) = delete;
    Function(Function &&obj) noexcept = delete;
    Function &operator=(const Function &obj) = delete;
    Function &operator=(Function &&obj) noexcept = delete;
    ~Function() = default;

    const std::string &getName() const;
    size_t getNumParams() const;
    const std::optional<RestParam> &getRestParam() const;
    const std::optional<Variable> &getReturnVar() const;
    Package &getPackageMutableRef() const;
    const Package &getPackageRef() const;
    const llvm::IRBuilder<> *getLLVMBuilder() const;
    llvm::Value *getLLVMValueForBranchComparison(const std::string &lhsName) const;
    llvm::AllocaInst *getLLVMLocalVar(const std::string &varName) const;
    llvm::Value *getLLVMLocalOrGlobalVar(const Operand &op, llvm::Module &module) const;
    const Variable &getLocalVariable(const std::string &opName) const;
    const Variable &getLocalOrGlobalVariable(const Operand &op) const;
    llvm::Type *getLLVMTypeOfReturnVal(llvm::Module &module) const;
    bool isMainFunction() const;
    bool isExternalFunction() const;
    const std::vector<FunctionParam> &getParams() const;
    llvm::Value *createTempVariable(const Operand &op, llvm::Module &module) const;

    void patchBasicBlocks();
    void insertParam(const FunctionParam &param);
    void setReturnVar(const Variable &var);
    void insertLocalVar(const Variable &var);
    void insertBasicBlock(const std::shared_ptr<BasicBlock> &bb);
    void insertBranchComparisonValue(const std::string &lhsName, llvm::Value *compRef);
    void setLLVMBuilder(llvm::IRBuilder<> *b);
    void storeValueInSmartStruct(llvm::Module &module, llvm::Value *value, const Type &valueType,
                                 llvm::Value *smartStruct);

    void translate(llvm::Module &module);
};
} // namespace nballerina

#endif //!__FUNCTION__H__
