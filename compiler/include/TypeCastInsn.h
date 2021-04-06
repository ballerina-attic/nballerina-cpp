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

#ifndef __TYPECASTINSN__H__
#define __TYPECASTINSN__H__

#include "NonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class Operand;

class TypeCastInsn : public NonTerminatorInsn {
  private:
    Operand rhsOp;
    LLVMValueRef getIsSameTypeDeclaration(LLVMModuleRef &modRef, LLVMValueRef lhsRef, LLVMValueRef rhsRef);
    LLVMValueRef isSameType(LLVMModuleRef &modRef, LLVMValueRef lhsRef, LLVMValueRef rhsRef);

  public:
    TypeCastInsn() = delete;
    TypeCastInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &rhsOp);
    ~TypeCastInsn() = default;
    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__TYPECASTINSN__H__
