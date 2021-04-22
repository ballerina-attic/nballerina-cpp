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

#ifndef __NONTERMINATORINSNCODEGEN__H__
#define __NONTERMINATORINSNCODEGEN__H__

#include "FunctionCodeGen.h"
#include "interfaces/Translatable.h"

namespace nballerina {
class NonTerminatorInsnCodeGen
    : public Translators<class ConstantLoadInsn, class TypeCastInsn, class StructureInsn, class BinaryOpInsn,
                         class MapStoreInsn, class MapLoadInsn, class ArrayInsn, class ArrayStoreInsn,
                         class ArrayLoadInsn, class UnaryOpInsn, class MoveInsn> {
  private:
    FunctionCodeGen &parentGenerator;

  public:
    NonTerminatorInsnCodeGen() = delete;
    NonTerminatorInsnCodeGen(FunctionCodeGen &parentGenerator) : parentGenerator(parentGenerator) {}
    ~NonTerminatorInsnCodeGen() = default;
    void visit(class ArrayInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class ArrayStoreInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class ArrayLoadInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class BinaryOpInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) override;
    void visit(class ConstantLoadInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class MapLoadInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class MapStoreInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class MoveInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) override;
    void visit(class StructureInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) override;
    void visit(class TypeCastInsn &obj, llvm::Module &module, llvm::IRBuilder<> &builder) override;
    void visit(class UnaryOpInsn &obj, llvm::Module &, llvm::IRBuilder<> &builder) override;
};

} // namespace nballerina

#endif //!__NONTERMINATORINSNCODEGEN__H__