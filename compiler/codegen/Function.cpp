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
#include "ConditionBrInsn.h"
#include "FunctionParam.h"
#include "InvocableType.h"
#include "Operand.h"
#include "Package.h"
#include "TerminatorInsn.h"
#include "Types.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace nballerina {

Function::Function(std::weak_ptr<Package> parentPackage, std::string name, std::string workerName, unsigned int flags)
    : parentPackage(std::move(parentPackage)), name(std::move(name)), workerName(std::move(workerName)), flags(flags),
      llvmBuilder(nullptr), llvmFunction(nullptr) {}

// Search basic block based on the basic block ID
std::weak_ptr<BasicBlock> Function::FindBasicBlock(const std::string &id) {
    const auto &bb = basicBlocksMap.find(id);
    assert(bb != basicBlocksMap.end());
    return bb->second;
}

const std::string &Function::getName() const { return name; }
const std::vector<FunctionParam> &Function::getParams() const { return requiredParams; }
const std::optional<RestParam> &Function::getRestParam() const { return restParam; }
const std::optional<Variable> &Function::getReturnVar() const { return returnVar; }
LLVMBuilderRef Function::getLLVMBuilder() const { return llvmBuilder; }
LLVMValueRef Function::getLLVMFunctionValue() const { return llvmFunction; }

LLVMValueRef Function::getLLVMValueForBranchComparison(const std::string &lhsName) const {
    const auto &branch = branchComparisonList.find(lhsName);
    if (branch == branchComparisonList.end()) {
        return nullptr;
    }
    return branch->second;
}

LLVMValueRef Function::getLLVMLocalVar(const std::string &varName) const {
    const auto &varIt = localVarRefs.find(varName);
    if (varIt == localVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

LLVMValueRef Function::createTempVariable(const Operand &operand) const {
    LLVMValueRef locVRef = getLLVMLocalOrGlobalVar(operand);
    std::string tempName = operand.getName() + "_temp";
    return LLVMBuildLoad(llvmBuilder, locVRef, tempName.c_str());
}

static bool isParamter(const Variable &locVar) {
    switch (locVar.getKind()) {
    case ARG_VAR_KIND:
        return true;
    default:
        return false;
    }
}

LLVMTypeRef Function::getLLVMTypeOfReturnVal() const {

    if (isMainFunction()) {
        return LLVMVoidType();
    }
    assert(!parentPackage.expired());
    return parentPackage.lock()->getLLVMTypeOfType(returnVar->getType());
}

void Function::insertParam(const FunctionParam &param) { requiredParams.push_back(param); }
void Function::insertLocalVar(const Variable &var) {
    localVars.insert(std::pair<std::string, Variable>(var.getName(), var));
}
void Function::setReturnVar(const Variable &var) { returnVar = var; }
void Function::insertBasicBlock(const std::shared_ptr<BasicBlock> &bb) {
    if (isBBMapEmpty) {
        firstBlock = bb;
        isBBMapEmpty = false;
    }
    basicBlocksMap.insert(std::pair<std::string, std::shared_ptr<BasicBlock>>(bb->getId(), bb));
}
void Function::setLLVMBuilder(LLVMBuilderRef b) { llvmBuilder = b; }
void Function::setLLVMFunctionValue(LLVMValueRef newFuncRef) { llvmFunction = newFuncRef; }
void Function::insertBranchComparisonValue(const std::string &name, LLVMValueRef compRef) {
    branchComparisonList.insert(std::pair<std::string, LLVMValueRef>(name, compRef));
}

const Variable &Function::getLocalVariable(const std::string &opName) const {
    const auto &varIt = localVars.find(opName);
    assert(varIt != localVars.end());
    return varIt->second;
}

const Variable &Function::getLocalOrGlobalVariable(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        assert(!parentPackage.expired());
        return parentPackage.lock()->getGlobalVariable(op.getName());
    }
    return getLocalVariable(op.getName());
}

LLVMValueRef Function::getLLVMLocalOrGlobalVar(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        assert(!parentPackage.expired());
        assert(parentPackage.lock()->getGlobalLLVMVar(op.getName()));
        return parentPackage.lock()->getGlobalLLVMVar(op.getName());
    }
    return getLLVMLocalVar(op.getName());
}

const Package &Function::getPackageRef() const {
    assert(!parentPackage.expired());
    return *parentPackage.lock();
}
Package &Function::getPackageMutableRef() const {
    assert(!parentPackage.expired());
    return *parentPackage.lock();
}

size_t Function::getNumParams() const { return requiredParams.size(); }

bool Function::isMainFunction() const { return (name == MAIN_FUNCTION_NAME); }

bool Function::isExternalFunction() const { return ((flags & NATIVE) == NATIVE); }

// Patches the Terminator Insn with destination Basic Block
void Function::patchBasicBlocks() {
    for (auto &basicBlock : basicBlocksMap) {
        auto *terminator = basicBlock.second->getTerminatorInsnPtr();
        if ((terminator == nullptr) || !terminator->isPatched()) {
            continue;
        }
        switch (terminator->getInstKind()) {
        case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
            auto *instruction = static_cast<ConditionBrInsn *>(terminator);
            auto trueBB = FindBasicBlock(instruction->getIfThenBBID());
            auto falseBB = FindBasicBlock(instruction->getElseBBID());
            instruction->setIfThenBB(trueBB);
            instruction->setElseBB(falseBB);
            instruction->setPatched();
            break;
        }
        case INSTRUCTION_KIND_GOTO: {
            auto destBB = FindBasicBlock(terminator->getNextBBID());
            terminator->setNextBB(destBB);
            terminator->setPatched();
            break;
        }
        case INSTRUCTION_KIND_CALL: {
            auto destBB = FindBasicBlock(terminator->getNextBBID());
            terminator->setNextBB(destBB);
            break;
        }
        default:
            llvm_unreachable("Invalid Insn Kind for Instruction Patching");
            break;
        }
    }
}

void Function::storeValueInSmartStruct(LLVMModuleRef &modRef, LLVMValueRef value, const Type &valueType,
                                       LLVMValueRef smartStruct) {

    // struct first element original type
    LLVMValueRef inherentTypeIdx = LLVMBuildStructGEP(llvmBuilder, smartStruct, 0, "inherentTypeName");

    // package function?
    std::string_view valueTypeName = Type::typeStringMangleName(valueType);
    assert(!parentPackage.expired());
    auto parent = parentPackage.lock();
    parent->addToStrTable(valueTypeName);
    int tempRandNum1 = std::rand() % 1000 + 1;
    LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum1, 0);
    LLVMValueRef valueTypeStoreRef = LLVMBuildStore(llvmBuilder, constValue, inherentTypeIdx);
    parent->addStringOffsetRelocationEntry(valueTypeName.data(), valueTypeStoreRef);

    // struct second element void pointer data.
    LLVMValueRef valuePtr = LLVMBuildStructGEP(llvmBuilder, smartStruct, 1, "data");
    if (isBoxValueSupport(valueType.getTypeTag())) {
        auto valueTemp = LLVMBuildLoad(llvmBuilder, value, "_temp");
        LLVMValueRef boxValFunc = generateBoxValueFunc(modRef, LLVMTypeOf(valueTemp), valueType.getTypeTag());
        value = LLVMBuildCall(llvmBuilder, boxValFunc, &valueTemp, 1, "call");
    }
    LLVMValueRef bitCastRes = LLVMBuildBitCast(llvmBuilder, value, LLVMPointerType(LLVMInt8Type(), 0), "");
    LLVMBuildStore(llvmBuilder, bitCastRes, valuePtr);
}

bool Function::isBoxValueSupport(TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT:
    case TYPE_TAG_FLOAT:
    case TYPE_TAG_BOOLEAN:
        return true;
    default:
        return false;
    }
}

LLVMValueRef Function::generateBoxValueFunc(LLVMModuleRef &modRef, LLVMTypeRef paramTypeRef, TypeTag typeTag) {
    std::string functionName = "box_bal_";
    functionName += Type::getNameOfType(typeTag);
    assert(!parentPackage.expired());
    auto parent = parentPackage.lock();
    LLVMValueRef boxValFuncRef = parent->getFunctionRef(functionName);
    if (boxValFuncRef != nullptr) {
        return boxValFuncRef;
    }
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(paramTypeRef, 0), &paramTypeRef, 1, 0);
    boxValFuncRef = LLVMAddFunction(modRef, functionName.c_str(), funcType);
    parent->addFunctionRef(functionName, boxValFuncRef);
    return boxValFuncRef;
}

void Function::translate(LLVMModuleRef &modRef) {

    LLVMBasicBlockRef BbRef = LLVMAppendBasicBlock(llvmFunction, "entry");
    LLVMPositionBuilderAtEnd(llvmBuilder, BbRef);
    assert(!parentPackage.expired());
    // iterate through all local vars.
    int paramIndex = 0;
    for (auto const &it : localVars) {
        const auto &locVar = it.second;
        LLVMTypeRef varType = parentPackage.lock()->getLLVMTypeOfType(locVar.getType());
        LLVMValueRef localVarRef = LLVMBuildAlloca(llvmBuilder, varType, (locVar.getName()).c_str());
        localVarRefs.insert({locVar.getName(), localVarRef});
        if (isParamter(locVar)) {
            LLVMValueRef parmRef = LLVMGetParam(llvmFunction, paramIndex);
            std::string paramName = requiredParams[paramIndex].getName();
            LLVMSetValueName2(parmRef, paramName.c_str(), paramName.length());
            if (parmRef != nullptr) {
                LLVMBuildStore(llvmBuilder, parmRef, localVarRef);
            }
            paramIndex++;
        }
    }

    // iterate through with each basic block in the function and create them
    for (auto &bb : basicBlocksMap) {
        LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(llvmFunction, bb.second->getId().c_str());
        bb.second->setLLVMBBRef(bbRef);
    }

    // creating branch to next basic block.
    if (auto firstBB = firstBlock.lock()) {
        if (firstBB->getLLVMBBRef() != nullptr) {
            LLVMBuildBr(llvmBuilder, firstBB->getLLVMBBRef());
        }
    }

    // Now translate the basic blocks (essentially add the instructions in them)
    for (auto &bb : basicBlocksMap) {
        LLVMPositionBuilderAtEnd(llvmBuilder, bb.second->getLLVMBBRef());
        bb.second->translate(modRef);
    }
    splitBBIfPossible(modRef);
}

LLVMValueRef Function::generateAbortInsn(LLVMModuleRef &modRef) {
    const char *abortFuncName = "abort";
    assert(!parentPackage.expired());
    auto parent = parentPackage.lock();
    LLVMValueRef abortFuncRef = parent->getFunctionRef(abortFuncName);
    if (abortFuncRef != nullptr) {
        return abortFuncRef;
    }
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), nullptr, 0, 0);
    abortFuncRef = LLVMAddFunction(modRef, abortFuncName, funcType);
    parent->addFunctionRef(abortFuncName, abortFuncRef);
    return abortFuncRef;
}

// Splitting Basicblock after 'is_same_type()' function call and
// based on is_same_type() function result, crating branch condition using
// splitBB Basicblock (ifBB) and abortBB(elseBB).
// In IfBB we are doing casing and from ElseBB Aborting.
void Function::splitBBIfPossible(LLVMModuleRef &modRef) {
    auto *llvmFunc = unwrap<llvm::Function>(llvmFunction);
    const char *isSameTypeChar = "is_same_type";
    for (auto &bBlock : *llvmFunc) {
        for (llvm::BasicBlock::iterator I = bBlock.begin(); I != bBlock.end(); ++I) {
            auto *callInst = dyn_cast<llvm::CallInst>(&*I);
            if (callInst == nullptr) {
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
            llvm::BasicBlock *splitBB = bBlock.splitBasicBlock(++I, bBlock.getName() + ".split");
            llvm::BasicBlock::iterator ILoc = bBlock.end();
            llvm::Instruction &lastInsn = *--ILoc;
            // branch intruction to the split BB is creating in BB2 (last BB)
            // basicblock, removing from BB2 and insert this branch instruction
            // into BB0(split original BB).
            lastInsn.removeFromParent();
            // Creating abortBB (elseBB).
            llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*unwrap(LLVMGetGlobalContext()), "abortBB");

            // Creating Branch condition using if and else BB's.
            llvm::Instruction *compInsnRef = unwrap(llvmBuilder)->CreateCondBr(compInsn, splitBB, elseBB);

            // branch to abortBB instruction is generating in last(e.g bb2 BB)
            // basicblock. here, moving from bb2 to bb0.split basicblock.
            compInsnRef->removeFromParent();
            bBlock.getInstList().push_back(compInsnRef);

            // get the last instruction from splitBB.
            llvm::BasicBlock::iterator SI = splitBB->end();
            assert(SI != splitBB->begin());
            auto &newBBLastInsn = *--SI;
            llvm::BasicBlock *elseBBSucc = newBBLastInsn.getSuccessor(0);
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
