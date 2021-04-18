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

#ifndef __CONSTANTLOAD__H__
#define __CONSTANTLOAD__H__

#include "NonTerminatorInsn.h"
#include "Types.h"
#include <string>
#include <variant>

namespace nballerina {

class ConstantLoadInsn : public NonTerminatorInsn {
  private:
    TypeTag typeTag;
    llvm::FunctionCallee getNewString(llvm::Module &module);
    std::variant<int64_t, float, bool, std::string> value;

  public:
    ConstantLoadInsn() = delete;
    ConstantLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, int64_t intVal);
    ConstantLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, float floatVal);
    ConstantLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, bool boolVal);
    ConstantLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB, std::string str);
    ConstantLoadInsn(const Operand &lhs, std::weak_ptr<BasicBlock> currentBB);
    ~ConstantLoadInsn() = default;

    void translate(llvm::Module &module, llvm::IRBuilder<> &builder) final;
};

} // namespace nballerina

#endif //!__CONSTANTLOAD__H__
