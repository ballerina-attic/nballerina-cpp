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

#ifndef __PACKAGE__H__
#define __PACKAGE__H__

#include "Function.h"
#include "Variable.h"
#include "interfaces/Translatable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/MC/StringTableBuilder.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class Type;

class Package : public Translatable {
  private:
    inline static const std::string BAL_NIL_VALUE = "bal_nil_value";
    inline static const std::string STRING_TABLE_NAME = "__string_table_ptr";
    std::string org;
    std::string name;
    std::string version;
    std::string sourceFileName;
    std::map<std::string, Variable> globalVars;
    std::map<std::string, LLVMValueRef> globalVarRefs;
    std::map<std::string, std::shared_ptr<Function>> functionLookUp;
    std::unique_ptr<llvm::StructType> boxType;
    std::unique_ptr<llvm::StringTableBuilder> strBuilder;
    std::map<std::string, std::vector<LLVMValueRef>> structElementStoreInst;
    std::map<std::string, LLVMValueRef> functionRefs;
    std::vector<std::unique_ptr<llvm::GlobalVariable>> globalStringValues;
    void applyStringOffsetRelocations(LLVMModuleRef &modRef);
    LLVMValueRef strBuilderGlobalPtr;

  public:
    Package() = default;
    ~Package() = default;
    Package(const Package &) = delete;
    Package(Package &&obj) noexcept = delete;
    Package &operator=(const Package &obj) = delete;
    Package &operator=(Package &&obj) noexcept = delete;

    const std::string &getOrgName() const;
    const std::string &getVersion() const;
    const std::string &getSrcFileName() const;
    const std::string &getPackageName() const;
    const Function &getFunction(const std::string &name) const;
    LLVMValueRef getGlobalLLVMVar(const std::string &globVar) const;
    LLVMTypeRef getLLVMTypeOfType(const Type &type) const;
    LLVMValueRef getFunctionRef(const std::string &arrayName) const;
    LLVMValueRef getGlobalNilVar() const;
    LLVMValueRef getStringBuilderTableGlobalPointer();
    // Return is optional only because we need to support _bal_resul
    // ToDo remove optional when support for _bal_result is removed
    std::optional<Variable> getGlobalVariable(const std::string &name) const;
    LLVMValueRef getMapIntStoreDeclaration(LLVMModuleRef &modRef);
    LLVMValueRef getMapSpreadFieldDeclaration(LLVMModuleRef &modRef);

    void addToStrTable(std::string_view name);
    void setOrgName(std::string orgName);
    void setPackageName(std::string pkgName);
    void setVersion(std::string verName);
    void setSrcFileName(std::string srcFileName);
    void insertGlobalVar(const Variable &var);
    void insertFunction(const std::shared_ptr<Function> &function);
    void addFunctionRef(const std::string &arrayName, LLVMValueRef functionRef);
    void addStringOffsetRelocationEntry(const std::string &eleType, LLVMValueRef storeInsn);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__PACKAGE__H__
