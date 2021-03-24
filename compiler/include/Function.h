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
#include "interfaces/Translatable.h"
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

class Function : public Debuggable, public Translatable {
  private:
    inline static const std::string MAIN_FUNCTION_NAME = "main";
    static constexpr unsigned int PUBLIC = 1;
    static constexpr unsigned int NATIVE = PUBLIC << 1;
    std::shared_ptr<Package> parentPackage;
    std::string name;
    std::string workerName;
    unsigned int flags;
    std::optional<Variable> returnVar;
    std::optional<RestParam> restParam;
    LLVMBuilderRef llvmBuilder;
    LLVMValueRef llvmFunction;
    std::shared_ptr<BasicBlock> firstBlock;
    std::map<std::string, Variable> localVars;
    std::map<std::string, std::shared_ptr<BasicBlock>> basicBlocksMap;
    std::map<std::string, LLVMValueRef> branchComparisonList;
    std::map<std::string, LLVMValueRef> localVarRefs;
    std::vector<FunctionParam> requiredParams;
    std::shared_ptr<BasicBlock> FindBasicBlock(const std::string &id);
    LLVMValueRef generateAbortInsn(LLVMModuleRef &modRef);
    void splitBBIfPossible(LLVMModuleRef &modRef);

  public:
    Function() = delete;
    Function(std::shared_ptr<Package> parentPackage, std::string name, std::string workerName, unsigned int flags);
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
    LLVMBuilderRef getLLVMBuilder() const;
    LLVMValueRef getLLVMFunctionValue() const;
    LLVMValueRef getLLVMValueForBranchComparison(const std::string &lhsName) const;
    LLVMValueRef getLLVMLocalVar(const std::string &varName) const;
    LLVMValueRef getLLVMLocalOrGlobalVar(const Operand &op) const;
    const Variable &getLocalVariable(const std::string &opName) const;
    std::optional<Variable> getLocalOrGlobalVariable(const Operand &op) const;
    LLVMTypeRef getLLVMTypeOfReturnVal() const;
    bool isMainFunction() const;
    bool isExternalFunction() const;
    const std::vector<FunctionParam> &getParams() const;
    LLVMValueRef createTempVariable(const Operand &op) const;

    void patchBasicBlocks();
    void insertParam(FunctionParam param);
    void setRestParam(RestParam param);
    void setReturnVar(const Variable &var);
    void insertLocalVar(const Variable &var);
    void insertBasicBlock(std::shared_ptr<BasicBlock> bb);
    void insertBranchComparisonValue(const std::string &lhsName, LLVMValueRef compRef);
    void setLLVMBuilder(LLVMBuilderRef builder);
    void setLLVMFunctionValue(LLVMValueRef funcRef);

    void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__FUNCTION__H__
