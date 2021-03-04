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

#ifndef __BALFUNCTION__H__
#define __BALFUNCTION__H__

#include "BalRestParam.h"
#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include "interfaces/Translatable.h"
#include <map>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;
class FunctionParam;
class Variable;
class InvokableType;
class RestParam;
class Type;
class Package;

class Function : public PackageNode, public Debuggable, public Translatable {
  private:
    Package *parentPackage;
    std::string name;
    std::string workerName;
    int flags;
    InvokableType *type;
    std::vector<FunctionParam *> requiredParams;
    Variable *receiver;
    RestParam *restParam;
    Variable *returnVar;
    std::map<std::string, Variable *> localVars;
    std::map<std::string, BasicBlock *> basicBlocksMap;
    std::vector<BasicBlock *> basicBlocks;
    LLVMBuilderRef llvmBuilder;
    LLVMValueRef llvmFunction;
    std::map<std::string, LLVMValueRef> branchComparisonList;
    std::map<std::string, LLVMValueRef> localVarRefs;

  public:
    Function() = delete;
    Function(Package *parentPackage, std::string name, std::string workerName, int pflags, InvokableType *ptype);
    Function(const Function &) = delete;
    ~Function() = default;

    FunctionParam *getParam(int index);
    std::string getName();
    size_t getNumParams();
    RestParam *getRestParam();
    Variable *getReturnVar();
    std::vector<BasicBlock *> getBasicBlocks();
    LLVMBuilderRef getLLVMBuilder();
    LLVMValueRef getLLVMFunctionValue();
    LLVMValueRef getLLVMValueForBranchComparison(std::string lhsName);
    BasicBlock *FindBasicBlock(std::string id);
    Package *getPackage() final;
    LLVMValueRef getTempLocalVariable(Operand *op);
    LLVMValueRef getLLVMLocalVar(std::string varName);
    LLVMValueRef getLLVMLocalOrGlobalVar(Operand *op);
    Variable *getLocalVariable(std::string opName);
    Variable *getLocalOrGlobalVariable(Operand *op);
    LLVMTypeRef getLLVMTypeOfReturnVal();

    void insertParam(FunctionParam *param);
    void setRestParam(RestParam *param);
    void setReceiver(Variable *var);
    void setReturnVar(Variable *var);
    void insertLocalVar(Variable *var);
    void insertBasicBlock(BasicBlock *bb);
    void insertBranchComparisonValue(std::string lhsName, LLVMValueRef compRef);
    void setLLVMBuilder(LLVMBuilderRef builder);
    void setLLVMFunctionValue(LLVMValueRef funcRef);

    void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALFUNCTION__H__
