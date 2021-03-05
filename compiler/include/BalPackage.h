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

#ifndef __BALPACKAGE__H__
#define __BALPACKAGE__H__

#include "interfaces/Translatable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/MC/StringTableBuilder.h"
#include <map>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class Function;
class Variable;
class Type;

class Package : public Translatable {
  private:
    std::string org;
    std::string name;
    std::string version;
    std::string sourceFileName;
    std::vector<Function *> functions;
    std::map<std::string, Variable *> globalVars;
    std::map<std::string, LLVMValueRef> globalVarRefs;
    std::map<std::string, Function *> functionLookUp;
    llvm::StructType *boxType;
    llvm::StringTableBuilder *strBuilder;
    std::map<std::string, std::vector<LLVMValueRef>> structElementStoreInst;
    std::map<std::string, LLVMValueRef> functionRefs;
    void applyStringOffsetRelocations();

  public:
    Package() = default;
    ~Package() = default;

    std::string getOrgName();
    std::string getPackageName();
    std::string getVersion();
    std::string getSrcFileName();
    Variable *getGlobalVariable(const std::string &name);
    LLVMValueRef getGlobalLLVMVar(const std::string &globVar);
    LLVMTypeRef getLLVMTypeOfType(Type *typeD);
    llvm::StringTableBuilder *getStrTableBuilder();
    Function *getFunction(const std::string &name);
    LLVMValueRef getFunctionRef(const std::string &arrayName);

    void setOrgName(std::string orgName);
    void setPackageName(std::string pkgName);
    void setVersion(std::string verName);
    void setSrcFileName(std::string srcFileName);
    void insertGlobalVar(Variable *var);
    void insertFunction(Function *function);
    void addFunctionRef(const std::string &, LLVMValueRef functionRef);
    void addStringOffsetRelocationEntry(const std::string &, LLVMValueRef storeInsn);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALPACKAGE__H__
