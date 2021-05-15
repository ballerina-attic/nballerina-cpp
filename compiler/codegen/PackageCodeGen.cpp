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

#include "codegen/PackageCodeGen.h"
#include "bir/FunctionParam.h"
#include "bir/Package.h"
#include "codegen/CodeGenUtils.h"
#include "codegen/FunctionCodeGen.h"
#include <iostream>

namespace nballerina {

PackageCodeGen::PackageCodeGen(llvm::Module &module)
    : module(module), globalStrTable(nullptr), globalStrTable2(nullptr) {}

llvm::Module &PackageCodeGen::getModule() { return module; }

void PackageCodeGen::visit(Package &obj, llvm::IRBuilder<> &builder) {

    module.setSourceFileName(obj.sourceFileName);

    llvm::Type *charPtrType = builder.getInt8PtrTy();
    llvm::Constant *nullValue = llvm::Constant::getNullValue(charPtrType);

    // String Table initialization
    strBuilder = std::make_unique<llvm::StringTableBuilder>(llvm::StringTableBuilder::RAW, 1);

    // creating external char pointer to store string builder table.
    globalStrTable = new llvm::GlobalVariable(module, charPtrType, false, llvm::GlobalValue::InternalLinkage, nullValue,
                                              STRING_TABLE_NAME, nullptr);
    globalStrTable->setAlignment(llvm::Align(4));

    // iterate over all global variables and translate
    for (auto const &globVar : obj.globalVars) {
        auto *varTyperef = CodeGenUtils::getLLVMTypeOfType(globVar.getType(), module);
        llvm::Constant *initValue = llvm::Constant::getNullValue(varTyperef);
        auto *gVar = new llvm::GlobalVariable(module, varTyperef, false, llvm::GlobalValue::ExternalLinkage, initValue,
                                              globVar.getName(), nullptr);
        gVar->setAlignment(llvm::Align(4));
    }

    // iterating over each function, first create function definition
    // (without function body) and adding to Module.
    for (const auto &function : obj.functions) {
        auto numParams = function.getNumParams();
        std::vector<llvm::Type *> paramTypes;
        paramTypes.reserve(numParams);
        for (const auto &funcParam : function.getParams()) {
            paramTypes.push_back(CodeGenUtils::getLLVMTypeOfType(funcParam.getType(), module));
        }

        bool isVarArg = static_cast<bool>(function.getRestParam());
        auto *funcType =
            llvm::FunctionType::get(FunctionCodeGen::getRetValType(function, module), paramTypes, isVarArg);

        llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, function.getName(), module);
    }

    // iterating over each function translate the function body
    for (auto &function : obj.functions) {
        if (function.isExternalFunction()) {
            continue;
        }
        FunctionCodeGen funcGenerator(*this);
        funcGenerator.visit(function, builder);
    }

    // This Api will finalize the string table builder if table size is not zero
    if (strBuilder->getSize() != 0) {
        applyStringOffsetRelocations(builder);
        // here, storing String builder table address into global char pointer.
        // like below example.
        // char arr[100] = { 'a' };
        // char *ptr = arr;
        auto *bitCastRes = builder.CreateBitCast(globalStrTable2, charPtrType, "");
        globalStrTable->setInitializer(llvm::dyn_cast<llvm::Constant>(bitCastRes));
    }
}

void PackageCodeGen::storeValueInSmartStruct(llvm::IRBuilder<> &builder, llvm::Value *value,
                                             [[maybe_unused]] const Type &valueType, llvm::Value *smartStruct) {

    // call the created int_to_any function
    const std::string functionName = "int_to_any";
    std::vector<llvm::Value *> paramValues;
    paramValues.reserve(1);
    llvm::LoadInst *inputValueRef = builder.CreateLoad(value, "");
    paramValues.push_back(inputValueRef);
    auto *namedFuncRef = module.getFunction(functionName);
    if (namedFuncRef == nullptr) {
        namedFuncRef = getIntToAnyCast(module, builder);
    }
    auto *callResult = builder.CreateCall(namedFuncRef, paramValues, "call");
    builder.CreateStore(callResult, smartStruct);
}

llvm::Function *PackageCodeGen::getIntToAnyCast(llvm::Module &module, llvm::IRBuilder<> &builder) {
    // create new int_to_any function
    std::vector<llvm::Type *> paramTypes;
    paramTypes.reserve(1);
    const std::string functionName = "int_to_any";
    paramTypes.push_back(builder.getInt64Ty());
    auto *funcType = llvm::FunctionType::get(builder.getInt8PtrTy(), paramTypes, false);
    auto *newFunc = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, functionName, module);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(module.getContext(), "entry");

    // create alloca of local variables in new function
    auto *localVarRef1 = builder.CreateAlloca(builder.getInt8PtrTy(), nullptr, "");
    auto *localVarRef2 = builder.CreateAlloca(builder.getInt64Ty(), nullptr, "");
    auto *localVarRef3 = builder.CreateAlloca(builder.getInt8PtrTy(), nullptr, "");
    auto *localVarRef4 = builder.CreateAlloca(llvm::Type::getInt64PtrTy(module.getContext()), nullptr, "");
    localVarRef1->removeFromParent();
    localVarRef2->removeFromParent();
    localVarRef3->removeFromParent();
    localVarRef4->removeFromParent();
    // insert into new entry BB
    entryBB->getInstList().push_back(localVarRef1);
    entryBB->getInstList().push_back(localVarRef2);
    entryBB->getInstList().push_back(localVarRef3);
    entryBB->getInstList().push_back(localVarRef4);
    llvm::Value *funcArg = &newFunc->arg_begin()[0];
    funcArg->setName("a");

    // creating instructions for entry BB in new function
    llvm::StoreInst *storeInputIntValue = builder.CreateStore(funcArg, localVarRef2);
    storeInputIntValue->removeFromParent();
    entryBB->getInstList().push_back(storeInputIntValue);

    llvm::LoadInst *valueLoad = builder.CreateLoad(localVarRef2, "");
    valueLoad->removeFromParent();
    entryBB->getInstList().push_back(valueLoad);

    auto *constIntValue = llvm::ConstantInt::get(builder.getInt64Ty(), 4611686018427387903);
    auto *ifReturn1 = new llvm::ICmpInst(llvm::CmpInst::Predicate::ICMP_SGE, constIntValue, valueLoad, "");
    entryBB->getInstList().push_back(ifReturn1);

    const std::string entryNextBBName = "lhs.true";
    const std::string ifElseBBName = "if.else";
    llvm::BasicBlock *ifBBEntry = llvm::BasicBlock::Create(module.getContext(), entryNextBBName);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(module.getContext(), ifElseBBName);
    auto *consBr1 = builder.CreateCondBr(ifReturn1, ifBBEntry, elseBB);
    consBr1->removeFromParent();
    entryBB->getInstList().push_back(consBr1);

    auto *constIntNegValue = llvm::ConstantInt::get(builder.getInt64Ty(), -4611686018427387904);
    auto *ifReturn2 = new llvm::ICmpInst(llvm::CmpInst::Predicate::ICMP_SGE, valueLoad, constIntNegValue, "");
    ifBBEntry->getInstList().push_back(ifReturn2);

    const std::string ifBBName = "if.then";
    llvm::BasicBlock *ifBB = llvm::BasicBlock::Create(module.getContext(), ifBBName);
    auto *consBr2 = builder.CreateCondBr(ifReturn2, ifBB, elseBB);
    consBr2->removeFromParent();
    ifBBEntry->getInstList().push_back(consBr2);

    const std::string returnBBName = "return";
    llvm::BasicBlock *retBB = llvm::BasicBlock::Create(module.getContext(), returnBBName);
    // insert all new basic blocks into new function int_to_any.
    newFunc->getBasicBlockList().push_back(entryBB);
    newFunc->getBasicBlockList().push_back(ifBBEntry);
    newFunc->getBasicBlockList().push_back(ifBB);
    newFunc->getBasicBlockList().push_back(elseBB);
    newFunc->getBasicBlockList().push_back(retBB);

    auto *constShlValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    llvm::BinaryOperator *shlInsn = llvm::BinaryOperator::CreateShl(valueLoad, constShlValue);
    shlInsn->setHasNoSignedWrap();

    llvm::BinaryOperator *orInsn = llvm::BinaryOperator::CreateOr(shlInsn, constShlValue);
    ifBB->getInstList().push_back(shlInsn);
    ifBB->getInstList().push_back(orInsn);

    auto *intToPtrCast = llvm::dyn_cast<llvm::Instruction>(builder.CreateIntToPtr(orInsn, builder.getInt8PtrTy()));
    intToPtrCast->removeFromParent();
    ifBB->getInstList().push_back(intToPtrCast);

    llvm::StoreInst *storeIfResToBalValue = builder.CreateStore(intToPtrCast, localVarRef1);
    storeIfResToBalValue->removeFromParent();
    ifBB->getInstList().push_back(storeIfResToBalValue);

    // creating branch of the if basicblock.
    llvm::Instruction *brInsn = builder.CreateBr(retBB);
    brInsn->removeFromParent();
    ifBB->getInstList().push_back(brInsn);

    // generate elseBB instructions
    auto *balValSize = llvm::ConstantInt::get(builder.getInt64Ty(), 8);
    auto *constByteValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);

    llvm::BinaryOperator *elseAddInsn = llvm::BinaryOperator::CreateAdd(balValSize, constByteValue, "");
    elseBB->getInstList().push_back(elseAddInsn);

    llvm::Instruction *mallocInsn = llvm::CallInst::CreateMalloc(elseBB, builder.getInt64Ty(), builder.getInt8Ty(),
                                                                 elseAddInsn, nullptr, nullptr, "");
    elseBB->getInstList().push_back(mallocInsn);

    llvm::StoreInst *storeMallocInsn = builder.CreateStore(mallocInsn, localVarRef3);
    storeMallocInsn->removeFromParent();
    elseBB->getInstList().push_back(storeMallocInsn);

    llvm::LoadInst *mallocResLoad = builder.CreateLoad(localVarRef3, "");
    mallocResLoad->removeFromParent();
    elseBB->getInstList().push_back(mallocResLoad);

    auto *gepOfMalloc = llvm::dyn_cast<llvm::Instruction>(builder.CreateInBoundsGEP(
        builder.getInt8Ty(), mallocResLoad, llvm::ArrayRef<llvm::Value *>({constByteValue}), ""));
    gepOfMalloc->removeFromParent();
    elseBB->getInstList().push_back(gepOfMalloc);

    auto *bitCastOfGEPMalloc = llvm::dyn_cast<llvm::Instruction>(
        builder.CreateBitCast(gepOfMalloc, llvm::Type::getInt64PtrTy(module.getContext()), ""));
    bitCastOfGEPMalloc->removeFromParent();
    elseBB->getInstList().push_back(bitCastOfGEPMalloc);

    llvm::StoreInst *storeBitCastRes = builder.CreateStore(bitCastOfGEPMalloc, localVarRef4);
    storeBitCastRes->removeFromParent();
    elseBB->getInstList().push_back(storeBitCastRes);

    llvm::LoadInst *locVarRefLoad = builder.CreateLoad(localVarRef2, "");
    locVarRefLoad->removeFromParent();
    elseBB->getInstList().push_back(locVarRefLoad);

    llvm::LoadInst *locVarRefLoad4 = builder.CreateLoad(localVarRef4, "");
    locVarRefLoad4->removeFromParent();
    elseBB->getInstList().push_back(locVarRefLoad4);

    llvm::StoreInst *storeValueToBalValue = builder.CreateStore(locVarRefLoad, locVarRefLoad4);
    storeValueToBalValue->removeFromParent();
    elseBB->getInstList().push_back(storeValueToBalValue);

    llvm::LoadInst *locVarRefLoad3 = builder.CreateLoad(localVarRef3, "");
    locVarRefLoad3->removeFromParent();
    elseBB->getInstList().push_back(locVarRefLoad3);

    llvm::StoreInst *storePointerValue = builder.CreateStore(locVarRefLoad3, localVarRef1);
    storePointerValue->removeFromParent();
    elseBB->getInstList().push_back(storePointerValue);

    llvm::Instruction *elseBBBrInsn = builder.CreateBr(retBB);
    elseBBBrInsn->removeFromParent();
    elseBB->getInstList().push_back(elseBBBrInsn);

    llvm::LoadInst *retValueRef = builder.CreateLoad(localVarRef1, "");
    retValueRef->removeFromParent();
    retBB->getInstList().push_back(retValueRef);

    llvm::Instruction *retValue = builder.CreateRet(retValueRef);
    retValue->removeFromParent();
    retBB->getInstList().push_back(retValue);
    return newFunc;
}

llvm::Value *PackageCodeGen::addToStringTable(std::string_view newString, llvm::IRBuilder<> &builder) {
    if (!strBuilder->contains(newString.data())) {
        strBuilder->add(newString.data());
    }
    int tempRandNum1 = std::rand() % 1000 + 1;
    auto *constValue = builder.getInt64(tempRandNum1);
    auto *strTblLoad = builder.CreateLoad(globalStrTable);
    auto *strTablePosition = builder.CreateInBoundsGEP(strTblLoad, llvm::ArrayRef<llvm::Value *>({constValue}));
    structElementStoreInst[newString.data()].push_back(strTablePosition);
    return strTablePosition;
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void PackageCodeGen::applyStringOffsetRelocations(llvm::IRBuilder<> &builder) {

    // finalizing the string builder table.
    strBuilder->finalize();
    // After finalize the string table, re arranging the actual offset values.
    std::vector<std::pair<size_t, std::string>> offsetStringPair;
    offsetStringPair.reserve(structElementStoreInst.size());

    for (const auto &element : structElementStoreInst) {
        const std::string &typeString = element.first;
        size_t finalOrigOffset = strBuilder->getOffset(element.first);
        offsetStringPair.emplace_back(finalOrigOffset, typeString);
    }

    // creating the concat string to store in the global address space(string table
    // global pointer)
    std::string concatString;
    std::sort(offsetStringPair.begin(), offsetStringPair.end());
    for (const auto &pair : offsetStringPair) {
        concatString.append(pair.second);
    }

    for (const auto &element : structElementStoreInst) {
        size_t finalOrigOffset = strBuilder->getOffset(element.first);
        auto *tempVal = builder.getInt64(finalOrigOffset);
        for (const auto &insn : element.second) {
            auto *GEPInst = llvm::dyn_cast<llvm::GetElementPtrInst>(insn);
            if (GEPInst != nullptr) {
                GEPInst->getOperand(1)->replaceAllUsesWith(tempVal);
                continue;
            }
            auto *temp = llvm::dyn_cast<llvm::User>(insn);
            if (temp != nullptr) {
                temp->getOperand(0)->replaceAllUsesWith(tempVal);
            } else {
                llvm_unreachable("");
            }
        }
    }

    auto *arrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(module.getContext()), concatString.size() + 1);
    globalStrTable2 = new llvm::GlobalVariable(module, arrayType, false, llvm::GlobalValue::ExternalLinkage, nullptr,
                                               STRING_TABLE_NAME, nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
    auto *constString = llvm::ConstantDataArray::getString(module.getContext(), concatString);
    // Initializing global address space with generated string(concat all the
    // strings from string builder table).
    globalStrTable2->setInitializer(constString);
}

} // namespace nballerina
