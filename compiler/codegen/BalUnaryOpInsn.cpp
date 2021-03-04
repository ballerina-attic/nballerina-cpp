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

#include "BalUnaryOpInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include <llvm-c/Core.h>
#include <string>

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {

UnaryOpInsn::UnaryOpInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp) {}

void UnaryOpInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    Operand *lhsOp = getLHS();
    string lhsTmpName = lhsOp->getName() + "_temp";
    LLVMValueRef lhsRef = funcObj->getLLVMLocalOrGlobalVar(lhsOp);
    LLVMValueRef rhsOpref = funcObj->getTempLocalVariable(rhsOp);
    LLVMValueRef ifReturn = LLVMBuildNot(builder, rhsOpref, lhsTmpName.c_str());
    LLVMBuildStore(builder, ifReturn, lhsRef);
    return;
}

} // namespace nballerina
