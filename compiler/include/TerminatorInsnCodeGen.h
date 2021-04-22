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

#ifndef __TERMINATORINSNCODEGEN__H__
#define __TERMINATORINSNCODEGEN__H__

#include "FunctionCodeGen.h"
#include "interfaces/Translatable.h"

namespace nballerina {

class TerminatorInsnCodeGen
    : public Translators<class ConditionBrInsn, class FunctionCallInsn, class GoToInsn, class ReturnInsn> {
  private:
    FunctionCodeGen &parentGenerator;

  public:
    TerminatorInsnCodeGen() = delete;
    TerminatorInsnCodeGen(FunctionCodeGen &parentGenerator);
    ~TerminatorInsnCodeGen() = default;
    void visit(class ConditionBrInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class FunctionCallInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class GoToInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) override;
    void visit(class ReturnInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) override;
};

} // namespace nballerina
#endif //!__TERMINATORINSNCODEGEN__H__
