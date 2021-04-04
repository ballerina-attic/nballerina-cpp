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

#ifndef __ERRORTYPEINSN__H__
#define __ERRORTYPEINSN__H__

#include "NonTerminatorInsn.h"
#include "Types.h"

namespace nballerina {

class ErrorTypeInsn : public NonTerminatorInsn {
  private:
    Operand msgOp;
    Operand causeOp;
    Operand detailOp;
    Type typeDecl;

  public:
    ErrorTypeInsn() = delete;
    ErrorTypeInsn(const Operand &lOp, std::shared_ptr<BasicBlock> currentBB, const Operand &mOp, const Operand &cOp, const Operand &dOp, Type tDecl);
    ~ErrorTypeInsn() = default;

    const Operand &getMsgOp();
    const Operand &getCauseOp();
    const Operand &getDetailOp();
    Type getTypeDecl();

    void setMsgOp(const Operand &op);
    void setCauseOp(const Operand &op);
    void setDetailOp(const Operand &op);
    void setTypeDecl(Type tDecl);
    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__ERRORTYPEINSN__H__
