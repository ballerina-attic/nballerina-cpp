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
#include "llvm/IR/GlobalVariable.h"
#include <string>

namespace nballerina {

// TODO Convert to template class
class ConstantLoadInsn : public NonTerminatorInsn {
  private:
    TypeTag typeTag;
    int intValue;
    double floatValue;
    bool boolValue;
    std::string strValue;
    std::unique_ptr<llvm::GlobalVariable> globalStringValue;

  public:
    ConstantLoadInsn() = delete;
    ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, int intVal);
    ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, float floatVal);
    ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, bool boolVal);
    ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, std::string str);
    ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB);
    ~ConstantLoadInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__CONSTANTLOAD__H__
