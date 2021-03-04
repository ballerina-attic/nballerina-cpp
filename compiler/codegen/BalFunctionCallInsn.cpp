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

#include "BalFunctionCallInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {

FunctionCallInsn::FunctionCallInsn(bool funcVirtual, std::string funcName, int argNumber, BasicBlock *nextBB,
                                   Operand *lhsOp, std::vector<Operand *> &fnArgs, BasicBlock *currentBB)
    : TerminatorInsn(lhsOp, currentBB, nextBB, true), isVirtual(funcVirtual), functionName(funcName),
      argCount(argNumber), argsList(std::move(fnArgs)) {
    kind = INSTRUCTION_KIND_CALL;
}

std::string FunctionCallInsn::getFunctionName() { return functionName; }
int FunctionCallInsn::getArgCount() { return argCount; }
Operand *FunctionCallInsn::getArgumentFromList(int i) { return argsList[i]; }

void FunctionCallInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef *ParamRefs = new LLVMValueRef[argCount];

    Function *function = getPackage()->getFunction(functionName);
    if (!function)
        llvm_unreachable("Unknown function call");

    for (int i = 0; i < argCount; i++) {
        Operand *op = argsList[i];
        LLVMValueRef opRef = funcObj->getTempLocalVariable(op);
        ParamRefs[i] = opRef;
    }

    LLVMValueRef lhsRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMValueRef namedFuncRef = function->getLLVMFunctionValue();
    LLVMValueRef callResult = LLVMBuildCall(builder, namedFuncRef, ParamRefs, argCount, "call");
    LLVMBuildStore(builder, callResult, lhsRef);

    // creating branch to next basic block.
    if (getNextBB())
        LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
}

} // namespace nballerina
