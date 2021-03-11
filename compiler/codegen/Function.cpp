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

#include "Function.h"
#include "BasicBlock.h"
#include "FunctionParam.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"

namespace nballerina {

Function::Function(Package *_parentPackage, std::string name, std::string workerName, [[maybe_unused]] int flags,
                   [[maybe_unused]] InvokableType *type)
    : parentPackage(_parentPackage), name(std::move(name)), workerName(std::move(workerName)), flags(flags),
      returnVar(nullptr), restParam(nullptr), receiver(nullptr), llvmBuilder(nullptr), llvmFunction(nullptr) {}

// Search basic block based on the basic block ID
BasicBlock *Function::FindBasicBlock(const std::string &id) {
    auto bb = basicBlocksMap.find(id);
    if (bb == basicBlocksMap.end()) {
        return nullptr;
    }
    return bb->second;
}

std::string Function::getName() { return name; }
FunctionParam *Function::getParam(int i) { return requiredParams[i]; }
RestParam *Function::getRestParam() { return restParam; }
Variable *Function::getReturnVar() { return returnVar; }
unsigned int Function::getFlags() { return flags; }
std::vector<BasicBlock *> Function::getBasicBlocks() { return basicBlocks; }
LLVMBuilderRef Function::getLLVMBuilder() { return llvmBuilder; }
LLVMValueRef Function::getLLVMFunctionValue() { return llvmFunction; }

LLVMValueRef Function::getLLVMValueForBranchComparison(const std::string &lhsName) {
    auto branch = branchComparisonList.find(lhsName);
    if (branch == branchComparisonList.end()) {
        return nullptr;
    }
    return branch->second;
}

LLVMValueRef Function::getLLVMLocalVar(const std::string &varName) {
    auto varIt = localVarRefs.find(varName);
    if (varIt == localVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

LLVMValueRef Function::getTempLocalVariable(Operand *operand) {
    LLVMValueRef locVRef = getLLVMLocalOrGlobalVar(operand);
    std::string tempName = operand->getName() + "_temp";
    return LLVMBuildLoad(llvmBuilder, locVRef, tempName.c_str());
}

static bool isParamter(Variable *locVar) {
    switch (locVar->getKind()) {
    case LOCAL_VAR_KIND:
    case TEMP_VAR_KIND:
    case RETURN_VAR_KIND:
    case GLOBAL_VAR_KIND:
    case SELF_VAR_KIND:
    case CONSTANT_VAR_KIND:
        return false;
    case ARG_VAR_KIND:
        return true;
    default:
        return false;
    }
}

LLVMTypeRef Function::getLLVMTypeOfReturnVal() {

    Type *type = returnVar->getTypeDecl();

    // if main function return type is void, but user wants to return some
    // value using _bal_result (global variable from BIR), change main function
    // return type from void to global variable (_bal_result) type.
    if (isMainFunction()) {
        assert(type->getTypeTag() == TYPE_TAG_NIL);
        Variable *globRetVar = parentPackage->getGlobalVariable("_bal_result");
        if (globRetVar == nullptr) {
            return LLVMVoidType();
        }
        return parentPackage->getLLVMTypeOfType(globRetVar->getTypeDecl());
    }
    return parentPackage->getLLVMTypeOfType(type);
}

void Function::insertParam(FunctionParam *param) { requiredParams.push_back(param); }
void Function::setReceiver(Variable *var) { receiver = var; }
void Function::setRestParam(RestParam *param) { restParam = param; }
void Function::insertLocalVar(Variable *var) {
    localVars.insert(std::pair<std::string, Variable *>(var->getName(), var));
}
void Function::setReturnVar(Variable *var) { returnVar = var; }
void Function::insertBasicBlock(BasicBlock *bb) {
    basicBlocks.push_back(bb);
    basicBlocksMap.insert(std::pair<std::string, BasicBlock *>(bb->getId(), bb));
}
void Function::setLLVMBuilder(LLVMBuilderRef b) { llvmBuilder = b; }
void Function::setLLVMFunctionValue(LLVMValueRef newFuncRef) { llvmFunction = newFuncRef; }
void Function::insertBranchComparisonValue(const std::string &name, LLVMValueRef compRef) {
    branchComparisonList.insert(std::pair<std::string, LLVMValueRef>(name, compRef));
}

Variable *Function::getLocalVariable(const std::string &opName) {
    auto varIt = localVars.find(opName);
    if (varIt == localVars.end()) {
        return nullptr;
    }
    return varIt->second;
}

Variable *Function::getLocalOrGlobalVariable(Operand *op) {
    if (op->getKind() == GLOBAL_VAR_KIND) {
        return parentPackage->getGlobalVariable(op->getName());
    }
    return getLocalVariable(op->getName());
}

LLVMValueRef Function::getLLVMLocalOrGlobalVar(Operand *op) {
    if (op->getKind() == GLOBAL_VAR_KIND) {
        return parentPackage->getGlobalLLVMVar(op->getName());
    }
    return getLLVMLocalVar(op->getName());
}

Package *Function::getPackage() { return parentPackage; }
size_t Function::getNumParams() { return requiredParams.size(); }

bool Function::isMainFunction() { return (name.compare(MAIN_FUNCTION_NAME) == 0); }

bool Function::isExternalFunction() { return ((flags & NATIVE) == NATIVE); }

void Function::translate(LLVMModuleRef &modRef) {

    LLVMBasicBlockRef BbRef = LLVMAppendBasicBlock(llvmFunction, "entry");
    LLVMPositionBuilderAtEnd(llvmBuilder, BbRef);

    // iterate through all local vars.
    int paramIndex = 0;
    for (auto const &it : localVars) {
        Variable *locVar = it.second;
        LLVMTypeRef varType = parentPackage->getLLVMTypeOfType(locVar->getTypeDecl());
        LLVMValueRef localVarRef = LLVMBuildAlloca(llvmBuilder, varType, (locVar->getName()).c_str());
        localVarRefs.insert({locVar->getName(), localVarRef});
        if (isParamter(locVar)) {
            LLVMValueRef parmRef = LLVMGetParam(llvmFunction, paramIndex);
            std::string paramName = getParam(paramIndex)->getName();
            LLVMSetValueName2(parmRef, paramName.c_str(), paramName.length());
            if (parmRef != nullptr) {
                LLVMBuildStore(llvmBuilder, parmRef, localVarRef);
            }
            paramIndex++;
        }
    }

    // iterate through with each basic block in the function and create them
    for (auto const &bb : basicBlocks) {
        LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(llvmFunction, bb->getId().c_str());
        bb->setLLVMBBRef(bbRef);
    }

    // creating branch to next basic block.
    if (!basicBlocks.empty() && (basicBlocks[0] != nullptr) && (basicBlocks[0]->getLLVMBBRef() != nullptr)) {
        LLVMBuildBr(llvmBuilder, basicBlocks[0]->getLLVMBBRef());
    }

    // Now translate the basic blocks (essentially add the instructions in them)
    for (auto const &bb : basicBlocks) {
        LLVMPositionBuilderAtEnd(llvmBuilder, bb->getLLVMBBRef());
        bb->translate(modRef);
    }
}

} // namespace nballerina
