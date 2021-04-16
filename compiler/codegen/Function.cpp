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
#include "Operand.h"
#include "Package.h"
#include "TerminatorInsn.h"
#include "Types.h"

namespace nballerina {

Function::Function(Package &parentPackage, std::string name, std::string workerName, unsigned int flags)
    : parentPackage(parentPackage), name(std::move(name)), workerName(std::move(workerName)), flags(flags),
      llvmBuilder(nullptr) {}

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

// IRBuilder should be in function? or pass to translate
const llvm::IRBuilder<> *Function::getLLVMBuilder() const { return llvmBuilder; }

llvm::Value *Function::getLLVMValueForBranchComparison(const std::string &lhsName) const {
    const auto &branch = branchComparisonList.find(lhsName);
    if (branch == branchComparisonList.end()) {
        return nullptr;
    }
    return branch->second;
}

llvm::AllocaInst *Function::getLLVMLocalVar(const std::string &varName) const {
    const auto &varIt = localVarRefs.find(varName);
    if (varIt == localVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

static bool isParamter(const Variable &locVar) {
    switch (locVar.getKind()) {
    case ARG_VAR_KIND:
        return true;
    default:
        return false;
    }
}

llvm::Type *Function::getLLVMTypeOfReturnVal(llvm::Module &module) const {
    if (isMainFunction()) {
        return llvm::Type::getVoidTy(module.getContext());
    }
    assert(returnVar.has_value());
    return parentPackage.getLLVMTypeOfType(returnVar->getType(), module);
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
void Function::setLLVMBuilder(llvm::IRBuilder<> *b) { llvmBuilder = b; }
void Function::insertBranchComparisonValue(const std::string &name, llvm::Value *compRef) {
    branchComparisonList.insert(std::pair<std::string, llvm::Value *>(name, compRef));
}

llvm::Value *Function::createTempVariable(const Operand &operand, llvm::Module &module) const {
    auto *variable = getLLVMLocalOrGlobalVar(operand, module);
    return llvmBuilder->CreateLoad(variable, operand.getName() + "_temp");
}

const Variable &Function::getLocalVariable(const std::string &opName) const {
    const auto &varIt = localVars.find(opName);
    assert(varIt != localVars.end());
    return varIt->second;
}

const Variable &Function::getLocalOrGlobalVariable(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        return parentPackage.getGlobalVariable(op.getName());
    }
    return getLocalVariable(op.getName());
}

llvm::Value *Function::getLLVMLocalOrGlobalVar(const Operand &op, llvm::Module &module) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        auto *variable = module.getGlobalVariable(op.getName(), false);
        assert(variable != nullptr);
        return variable;
    }
    return getLLVMLocalVar(op.getName());
}

const Package &Function::getPackageRef() const { return parentPackage; }

// TODO mutable reference shouldn't be required
Package &Function::getPackageMutableRef() const { return parentPackage; }

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

// package function?
void Function::storeValueInSmartStruct(llvm::Module &module, llvm::Value *value, const Type &valueType,
                                       llvm::Value *smartStruct) {

    // struct first element original type
    auto *inherentTypeIdx = llvmBuilder->CreateStructGEP(smartStruct, 0, "inherentTypeName");
    auto valueTypeName = Type::typeStringMangleName(valueType);
    parentPackage.addToStrTable(valueTypeName);
    int tempRandNum1 = std::rand() % 1000 + 1;
    auto *constValue = llvmBuilder->getInt64(tempRandNum1);
    auto *storeInsn = llvmBuilder->CreateStore(constValue, inherentTypeIdx);
    parentPackage.addStringOffsetRelocationEntry(valueTypeName.data(), storeInsn);

    // struct second element void pointer data.
    auto *valueIndx = llvmBuilder->CreateStructGEP(smartStruct, 1, "data");
    if (isBoxValueSupport(valueType.getTypeTag())) {
        auto *valueTemp = llvmBuilder->CreateLoad(value, "_temp");
        auto boxValFunc = generateBoxValueFunc(module, valueTemp->getType(), valueType.getTypeTag());
        value = llvmBuilder->CreateCall(boxValFunc, llvm::ArrayRef<llvm::Value *>({valueTemp}), "call");
    }
    auto *bitCastRes = llvmBuilder->CreateBitCast(value, llvmBuilder->getInt8PtrTy(), "");
    llvmBuilder->CreateStore(bitCastRes, valueIndx);
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

llvm::FunctionCallee Function::generateBoxValueFunc(llvm::Module &module, llvm::Type *paramType, TypeTag typeTag) {
    const std::string functionName = "box_bal_" + Type::getNameOfType(typeTag);
    auto *funcType =
        llvm::FunctionType::get(llvm::PointerType::get(paramType, 0), llvm::ArrayRef<llvm::Type *>({paramType}), false);
    return module.getOrInsertFunction(functionName, funcType);
}

void Function::translate(llvm::Module &module) {

    auto *llvmFunction = module.getFunction(name);
    auto *BbRef = llvm::BasicBlock::Create(module.getContext(), "entry", llvmFunction);
    llvmBuilder->SetInsertPoint(BbRef);

    // iterate through all local vars.
    size_t paramIndex = 0;
    for (auto const &it : localVars) {
        const auto &locVar = it.second;
        auto *varType = parentPackage.getLLVMTypeOfType(locVar.getType(), module);
        auto *localVarRef = llvmBuilder->CreateAlloca(varType, nullptr, locVar.getName());
        localVarRefs.insert({locVar.getName(), localVarRef});

        if (isParamter(locVar)) {
            llvm::Argument *parmRef = &(llvmFunction->arg_begin()[paramIndex]);
            auto paramName = requiredParams[paramIndex].getName();
            parmRef->setName(paramName);
            llvmBuilder->CreateStore(parmRef, localVarRef);
            paramIndex++;
        }
    }

    // iterate through with each basic block in the function and create them
    for (auto &bb : basicBlocksMap) {
        bb.second->setLLVMBBRef(llvm::BasicBlock::Create(module.getContext(), bb.second->getId(), llvmFunction));
    }

    // creating branch to next basic block.
    if (auto firstBB = firstBlock.lock()) {
        llvmBuilder->CreateBr(firstBB->getLLVMBBRef());
    }

    // Now translate the basic blocks (essentially add the instructions in them)
    for (auto &bb : basicBlocksMap) {
        llvmBuilder->SetInsertPoint(bb.second->getLLVMBBRef());
        bb.second->translate(module);
    }
    splitBBIfPossible(module);
}

llvm::FunctionCallee Function::generateAbortInsn(llvm::Module &module) {
    const std::string abortFuncName = "abort";
    auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()), false);
    return module.getOrInsertFunction(abortFuncName, funcType);
}

// Splitting Basicblock after 'is_same_type()' function call and
// based on is_same_type() function result, crating branch condition using
// splitBB Basicblock (ifBB) and abortBB(elseBB).
// In IfBB we are doing casing and from ElseBB Aborting.
void Function::splitBBIfPossible(llvm::Module &module) {
    const char *isSameTypeChar = "is_same_type";
    auto *llvmFunction = module.getFunction(name);
    for (auto &bBlock : *llvmFunction) {
        for (llvm::BasicBlock::iterator I = bBlock.begin(); I != bBlock.end(); ++I) {
            auto *callInst = llvm::dyn_cast<llvm::CallInst>(&*I);
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
            llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(module.getContext(), "abortBB");

            // Creating Branch condition using if and else BB's.
            llvm::Instruction *compInsnRef = llvmBuilder->CreateCondBr(compInsn, splitBB, elseBB);

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
            llvm::Instruction *brInsn = llvmBuilder->CreateBr(elseBBSucc);
            brInsn->removeFromParent();

            // generate LLVMFunction call to Abort from elseLLVMBB(abortBB).
            auto abortFunc = generateAbortInsn(module);
            auto abortFuncCallInsn = llvmBuilder->CreateCall(abortFunc);
            abortFuncCallInsn->removeFromParent();

            // Inserting Abort Functioncall instruction into elseLLVMBB(abortBB).
            elseBB->getInstList().push_back(abortFuncCallInsn);
            elseBB->getInstList().push_back(brInsn);

            // Inserting elseLLVMBB (abort BB) after splitBB (bb0.split)
            // basicblock.
            splitBB->getParent()->getBasicBlockList().insertAfter(splitBB->getIterator(), elseBB);
            break;
        }
    }
}
} // namespace nballerina
