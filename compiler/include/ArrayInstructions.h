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

#ifndef __ARRAYINSNS__H__
#define __ARRAYINSNS__H__

#include "NonTerminatorInsn.h"
#include "Types.h"

namespace nballerina {

// Forward Declaration
class Type;

class ArrayInsn : public NonTerminatorInsn {
  private:
    Operand *sizeOp;
    LLVMValueRef getArrayInitDeclaration(LLVMModuleRef &modRef);

  public:
    ArrayInsn() = delete;
    ArrayInsn(Operand *lOp, BasicBlock *currentBB, Operand *sOp, Type *TDecl);
    ~ArrayInsn() = default;
    void translate(LLVMModuleRef &modRef) final;
};

class ArrayLoadInsn : public NonTerminatorInsn {
  private:
    Operand *keyOp;
    Operand *rhsOp;
    LLVMValueRef getArrayLoadDeclaration(LLVMModuleRef &modRef, TypeTag lhsOpTypeTag);

  public:
    ArrayLoadInsn() = delete;
    ArrayLoadInsn(Operand *lOp, BasicBlock *currentBB, bool optionalFieldAccess, bool fillingRead, Operand *KOp,
                  Operand *ROp);
    ~ArrayLoadInsn() = default;
    void translate(LLVMModuleRef &modRef) final;
};

class ArrayStoreInsn : public NonTerminatorInsn {
  private:
    Operand *keyOp;
    Operand *rhsOp;
    LLVMValueRef getArrayStoreDeclaration(LLVMModuleRef &modRef, TypeTag rhsOpTypeTag);

  public:
    ArrayStoreInsn() = delete;
    ArrayStoreInsn(Operand *lOp, BasicBlock *currentBB, Operand *KOp, Operand *ROp);
    ~ArrayStoreInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__ARRAYINSNS__H__
