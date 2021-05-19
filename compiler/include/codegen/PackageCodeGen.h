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

#ifndef __PACKAGECODEGEN__H__
#define __PACKAGECODEGEN__H__

#include "bir/Types.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/StringTableBuilder.h>

namespace nballerina {

class PackageCodeGen {
  private:
    inline static const std::string BAL_NIL_VALUE = "bal_nil_value";
    inline static const std::string STRING_TABLE_NAME = "__string_table_ptr";
    llvm::Module &module;
    llvm::GlobalVariable *globalStrTable;
    llvm::GlobalVariable *globalStrTable2;
    std::unique_ptr<llvm::StringTableBuilder> strBuilder;
    std::map<std::string, std::vector<llvm::Value *>> structElementStoreInst;
    void applyStringOffsetRelocations(llvm::IRBuilder<> &builder);

  public:
    PackageCodeGen() = delete;
    PackageCodeGen(llvm::Module &module);
    ~PackageCodeGen() = default;

    llvm::Module &getModule();
    llvm::Value *addToStringTable(std::string_view newString, llvm::IRBuilder<> &builder);

    void visit(class Package &obj, llvm::IRBuilder<> &builder);
};

} // namespace nballerina

#endif //!__PACKAGECODEGEN__H__
