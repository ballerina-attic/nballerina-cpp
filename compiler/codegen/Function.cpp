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
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace nballerina {

Function::Function(Package *_parentPackage, std::string name, std::string workerName, [[maybe_unused]] int flags,
                   [[maybe_unused]] InvokableType *type)
    : parentPackage(_parentPackage), name(std::move(name)), workerName(std::move(workerName)), returnVar(nullptr),
      restParam(nullptr), receiver(nullptr), llvmBuilder(nullptr), llvmFunction(nullptr) {}

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
    splitBBIfPossible(modRef);
}

LLVMValueRef Function::generateAbortInsn(LLVMModuleRef &modRef) {
    const char *abortFuncName = "abort";
    LLVMValueRef abortFuncRef = getPackage()->getFunctionRef(abortFuncName);
    if (abortFuncRef != nullptr) {
        return abortFuncRef;
    }
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), nullptr, 0, 0);
    abortFuncRef = LLVMAddFunction(modRef, abortFuncName, funcType);
    getPackage()->addFunctionRef(abortFuncName, abortFuncRef);
    return abortFuncRef;
}

// Splitting Basicblock after 'is_same_type()' function call and
// based on is_same_type() function result, crating branch condition using
// splitBB Basicblock (ifBB) and abortBB(elseBB).
// In IfBB we are doing casing and from ElseBB Aborting.
void Function::splitBBIfPossible(LLVMModuleRef &modRef) {
    llvm::Function *llvmFunc = unwrap<llvm::Function>(llvmFunction);
    const char *isSameTypeChar = "is_same_type";
    for (llvm::Function::iterator FI = llvmFunc->begin(), FE = llvmFunc->end(); FI != FE; ++FI) {

        llvm::BasicBlock *bBlock = &*FI;
        for (llvm::BasicBlock::iterator I = bBlock->begin(); I != bBlock->end(); ++I) {
            llvm::CallInst *callInst = dyn_cast<llvm::CallInst>(&*I);
            if (!callInst) {
                continue;
            }
            size_t totalOperands = callInst->getNumOperands();
            const char *insnName = callInst->getOperand(totalOperands - 1)->getName().data();
            if (strcmp(insnName, isSameTypeChar) != 0) {
                continue;
            }
            std::advance(I, 1);
            llvm::Instruction *compInsn = &*I;
            // Splitting BasicBlock.
            llvm::BasicBlock *splitBB = bBlock->splitBasicBlock(++I, bBlock->getName() + ".split");
            llvm::BasicBlock::iterator ILoc = bBlock->end();
            llvm::Instruction *lastInsn = (&*--ILoc);
            // branch intruction to the split BB is creating in BB2 (last BB)
            // basicblock, removing from BB2 and insert this branch instruction
            // into BB0(split original BB).
            lastInsn->removeFromParent();
            // Creating abortBB (elseBB).
            llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*unwrap(LLVMGetGlobalContext()), "abortBB");

            // Creating Branch condition using if and else BB's.
            llvm::Instruction *compInsnRef = unwrap(llvmBuilder)->CreateCondBr(compInsn, splitBB, elseBB);

            // branch to abortBB instruction is generating in last(e.g bb2 BB)
            // basicblock. here, moving from bb2 to bb0.split basicblock.
            compInsnRef->removeFromParent();
            bBlock->getInstList().push_back(compInsnRef);

            // get the last instruction from splitBB.
            llvm::Instruction *newBBLastInsn = nullptr;
            llvm::BasicBlock::iterator SI = splitBB->end();
            if (SI != splitBB->begin())
                newBBLastInsn = &*--SI;
            assert(newBBLastInsn);
            llvm::BasicBlock *elseBBSucc = newBBLastInsn->getSuccessor(0);
            // creating branch to else basicblock.
            llvm::Instruction *brInsn = unwrap(llvmBuilder)->CreateBr(elseBBSucc);
            brInsn->removeFromParent();
            // generate LLVMFunction call to Abort from elseLLVMBB(abortBB).
            LLVMValueRef abortInsn = generateAbortInsn(modRef);
            LLVMValueRef abortFuncCallInsn = LLVMBuildCall(llvmBuilder, abortInsn, nullptr, 0, "");
            LLVMInstructionRemoveFromParent(abortFuncCallInsn);
            // Inserting Abort Functioncall instruction into elseLLVMBB(abortBB).
            elseBB->getInstList().push_back(unwrap<llvm::Instruction>(abortFuncCallInsn));
            elseBB->getInstList().push_back(brInsn);
            // Inserting elseLLVMBB (abort BB) after splitBB (bb0.split)
            // basicblock.
            splitBB->getParent()->getBasicBlockList().insertAfter(splitBB->getIterator(), elseBB);
            break;
        }
    }
}
} // namespace nballerina
