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

#include "codegen/FunctionCodeGen.h"
#include "bir/BasicBlock.h"
#include "bir/Function.h"
#include "bir/FunctionParam.h"
#include "codegen/BasicBlockCodeGen.h"
#include "codegen/CodeGenUtils.h"

namespace nballerina {

FunctionCodeGen::FunctionCodeGen(PackageCodeGen &parentGenerator) : parentGenerator(parentGenerator) {}

llvm::BasicBlock *FunctionCodeGen::getBasicBlock(const std::string &id) { return basicBlocksMap[id]; }

llvm::AllocaInst *FunctionCodeGen::getLocalVal(const std::string &varName) const {
    const auto &varIt = localVarRefs.find(varName);
    if (varIt == localVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

llvm::Type *FunctionCodeGen::getRetValType(const Function &obj, llvm::Module &module) {
    if (obj.isMainFunction()) {
        return llvm::Type::getVoidTy(module.getContext());
    }
    assert(obj.returnVar.has_value());
    return CodeGenUtils::getLLVMTypeOfType(obj.returnVar->getType(), module);
}

llvm::Value *FunctionCodeGen::createTempVal(const Operand &operand, llvm::IRBuilder<> &builder) const {
    auto *variable = getLocalOrGlobalVal(operand);
    return builder.CreateLoad(variable, operand.getName() + "_temp");
}

llvm::Value *FunctionCodeGen::getLocalOrGlobalVal(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        auto *variable = parentGenerator.getModule().getGlobalVariable(op.getName(), false);
        assert(variable != nullptr);
        return variable;
    }
    return getLocalVal(op.getName());
}

llvm::Function *FunctionCodeGen::getFunctionValue() { return llvmFunction; }

void FunctionCodeGen::visit(Function &obj, llvm::IRBuilder<> &builder) {

    llvm::Module &module = parentGenerator.getModule();
    llvmFunction = module.getFunction(obj.name);
    auto *BbRef = llvm::BasicBlock::Create(module.getContext(), "entry", llvmFunction);
    builder.SetInsertPoint(BbRef);

    // iterate through all local vars.
    size_t paramIndex = 0;
    for (auto const &it : obj.localVars) {
        const auto &locVar = it.second;
        auto *varType = CodeGenUtils::getLLVMTypeOfType(locVar.getType(), module);
        auto *localVarRef = builder.CreateAlloca(varType, nullptr, locVar.getName());
        localVarRefs.insert({locVar.getName(), localVarRef});

        if (locVar.isParamter()) {
            llvm::Argument *parmRef = &(llvmFunction->arg_begin()[paramIndex]);
            auto paramName = obj.requiredParams[paramIndex].getName();
            parmRef->setName(paramName);
            builder.CreateStore(parmRef, localVarRef);
            paramIndex++;
        }
    }

    // iterate through with each basic block in the function and create them
    for (auto &bb : obj.basicBlocks) {
        basicBlocksMap[bb->getId()] = llvm::BasicBlock::Create(module.getContext(), bb->getId(), llvmFunction);
    }

    // creating branch to next basic block.
    if (!obj.basicBlocks.empty()) {
        builder.CreateBr(basicBlocksMap[obj.basicBlocks[0]->getId()]);
    }

    // Now translate the basic blocks (essentially add the instructions in them)
    for (auto &bb : obj.basicBlocks) {
        builder.SetInsertPoint(basicBlocksMap[bb->getId()]);
        BasicBlockCodeGen generator(*this, parentGenerator);
        generator.visit(*bb, builder);
    }
}
} // namespace nballerina
