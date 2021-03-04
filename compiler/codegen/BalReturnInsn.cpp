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

#include "BalReturnInsn.h"
#include "BalFunction.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;
using namespace llvm;

namespace nballerina {

ReturnInsn::ReturnInsn(BasicBlock *currentBB) : TerminatorInsn(nullptr, currentBB, nullptr, false) {}

void ReturnInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef globRetRef = getPackage()->getGlobalLLVMVar("_bal_result");

    if (funcObj->getName() == "main" && globRetRef) {
        LLVMValueRef retValRef = LLVMBuildLoad(builder, globRetRef, "retrun_temp");
        LLVMBuildRet(builder, retValRef);
        return;
    }

    if (funcObj->getReturnVar()->getTypeDecl()->getTypeTag() == TYPE_TAG_NIL) {
        LLVMBuildRetVoid(builder);
        return;
    }

    LLVMValueRef retValueRef = LLVMBuildLoad(builder, funcObj->getLLVMLocalVar("%0"), "retrun_temp");
    LLVMBuildRet(builder, retValueRef);
}

} // namespace nballerina
