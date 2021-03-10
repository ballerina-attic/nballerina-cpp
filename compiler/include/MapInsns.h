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

#ifndef __MAPSTOREINSN__H__
#define __MAPSTOREINSN__H__

#include "NonTerminatorInsn.h"

namespace nballerina {
class MapStoreInsn : public NonTerminatorInsn {
  private:
    Operand keyOp;
    Operand rhsOp;
    LLVMValueRef getMapIntStoreDeclaration(LLVMModuleRef &modRef);

  public:
    MapStoreInsn() = delete;
    MapStoreInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &KOp, const Operand &ROp);
    ~MapStoreInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__MAPSTOREINSN__H__
