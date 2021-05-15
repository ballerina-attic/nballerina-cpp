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

#include "bir/Function.h"
#include "bir/Operand.h"
#include "bir/TypeCastInsn.h"
#include "bir/Types.h"
#include "bir/Variable.h"
#include "codegen/CodeGenUtils.h"
#include "codegen/NonTerminatorInsnCodeGen.h"

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(class TypeCastInsn &obj, llvm::IRBuilder<> &builder) {
    const auto &funcObj = obj.getFunctionRef();
    auto *rhsOpRef = functionGenerator.getLocalOrGlobalVal(obj.rhsOp);
    auto *lhsOpRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    auto *lhsTypeRef = lhsOpRef->getType();

    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(obj.lhsOp);
    const auto &rhsVar = funcObj.getLocalOrGlobalVariable(obj.rhsOp);
    const auto &lhsType = lhsVar.getType();
    auto lhsTypeTag = lhsType.getTypeTag();
    const auto &rhsType = rhsVar.getType();
    auto rhsTypeTag = rhsType.getTypeTag();
    auto &module = moduleGenerator.getModule();

    if (Type::isSmartStructType(rhsTypeTag)) {
        if (Type::isSmartStructType(lhsTypeTag)) {
            auto *rhsVarOpRef = functionGenerator.createTempVal(obj.rhsOp, builder);
            builder.CreateStore(rhsVarOpRef, lhsOpRef);
            return;
        }
        // call the any_to_int function to typecast from any to int type.
        std::vector<llvm::Value *> paramValues;
        paramValues.reserve(1);
        llvm::LoadInst *rhsValueRef = builder.CreateLoad(rhsOpRef, "");
        paramValues.push_back(rhsValueRef);
        auto *namedFuncRef = module.getFunction("any_to_int");
        if (namedFuncRef == nullptr) {
            namedFuncRef = getAnyToIntCast(module, builder);
        }
        auto *callResult = builder.CreateCall(namedFuncRef, paramValues, "call");
        builder.CreateStore(callResult, lhsOpRef);
    } else if (Type::isSmartStructType(lhsTypeTag)) {
        moduleGenerator.storeValueInSmartStruct(builder, rhsOpRef, rhsType, lhsOpRef);
    } else if (lhsTypeTag == TYPE_TAG_INT && rhsTypeTag == TYPE_TAG_FLOAT) {
        auto *rhsLoad = builder.CreateLoad(rhsOpRef);
        auto *lhsLoad = builder.CreateLoad(lhsOpRef);
        auto *castResult = builder.CreateFPToSI(rhsLoad, lhsLoad->getType(), "");
        builder.CreateStore(castResult, lhsOpRef);
    } else {
        builder.CreateBitCast(rhsOpRef, lhsTypeRef, "data_cast");
    }
}

llvm::Function *NonTerminatorInsnCodeGen::getAnyToIntCast(llvm::Module &module, llvm::IRBuilder<> &builder) {

    // create new any_to_int function
    std::vector<llvm::Type *> paramTypes;
    paramTypes.reserve(1);
    paramTypes.push_back(builder.getInt8PtrTy());
    auto *funcType = llvm::FunctionType::get(builder.getInt64Ty(), paramTypes, false);
    auto *newFunc = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, "any_to_int", module);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(module.getContext(), "entry");

    // create alloca of local variables
    auto *localVarRef1 = builder.CreateAlloca(builder.getInt64Ty(), nullptr, "");
    auto *localVarRef2 = builder.CreateAlloca(builder.getInt8PtrTy(), nullptr, "");
    auto *localVarRef3 = builder.CreateAlloca(llvm::Type::getInt64PtrTy(module.getContext()), nullptr, "");
    localVarRef1->removeFromParent();
    localVarRef2->removeFromParent();
    localVarRef3->removeFromParent();
    // insert alloca insns to entry BB
    entryBB->getInstList().push_back(localVarRef1);
    entryBB->getInstList().push_back(localVarRef2);
    entryBB->getInstList().push_back(localVarRef3);

    llvm::Value *funcArg = &newFunc->arg_begin()[0];
    funcArg->setName("a");

    llvm::StoreInst *storeInputIntValue = builder.CreateStore(funcArg, localVarRef2);
    storeInputIntValue->removeFromParent();
    entryBB->getInstList().push_back(storeInputIntValue);

    llvm::LoadInst *valueLoad = builder.CreateLoad(localVarRef2, "");
    valueLoad->removeFromParent();
    entryBB->getInstList().push_back(valueLoad);

    auto *ptrToIntCast = llvm::dyn_cast<llvm::Instruction>(builder.CreatePtrToInt(valueLoad, builder.getInt64Ty(), ""));
    ptrToIntCast->removeFromParent();
    entryBB->getInstList().push_back(ptrToIntCast);

    auto *constOneValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    llvm::Instruction *andInsn = llvm::BinaryOperator::CreateAnd(ptrToIntCast, constOneValue);
    entryBB->getInstList().push_back(andInsn);

    auto *constZeroValue = llvm::ConstantInt::get(builder.getInt64Ty(), 0);
    llvm::Instruction *ifReturn = new llvm::ICmpInst(llvm::CmpInst::Predicate::ICMP_NE, andInsn, constZeroValue, "");
    entryBB->getInstList().push_back(ifReturn);

    // create if BB
    llvm::BasicBlock *ifBB = llvm::BasicBlock::Create(module.getContext(), "if.then");
    // create else BB
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(module.getContext(), "if.else");
    // Creating Branch condition using if and else BB's.
    auto *condBr = builder.CreateCondBr(ifReturn, ifBB, elseBB);
    condBr->removeFromParent();
    entryBB->getInstList().push_back(condBr);

    llvm::BinaryOperator *shrInsn = llvm::BinaryOperator::CreateAShr(ptrToIntCast, constOneValue);
    ifBB->getInstList().push_back(shrInsn);

    llvm::StoreInst *storeValueToBalValue = builder.CreateStore(shrInsn, localVarRef1);
    storeValueToBalValue->removeFromParent();
    ifBB->getInstList().push_back(storeValueToBalValue);

    llvm::BasicBlock *retBB = llvm::BasicBlock::Create(module.getContext(), "return");
    // creating branch of the if basicblock.
    llvm::Instruction *brInsn = builder.CreateBr(retBB);
    brInsn->removeFromParent();
    ifBB->getInstList().push_back(brInsn);

    auto *constTagMask = llvm::ConstantInt::get(builder.getInt64Ty(), 0b11);
    llvm::Instruction *andInsnElseBB = llvm::BinaryOperator::CreateAnd(ptrToIntCast, constTagMask);
    elseBB->getInstList().push_back(andInsnElseBB);

    auto *compareResult = llvm::dyn_cast<llvm::Instruction>(
        builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, andInsnElseBB, constZeroValue, ""));
    compareResult->removeFromParent();
    elseBB->getInstList().push_back(compareResult);

    // create if BB in else BB
    llvm::BasicBlock *elseIfBB = llvm::BasicBlock::Create(module.getContext(), "if.then2");
    // create else BB in else BB
    llvm::BasicBlock *elseElseBB = llvm::BasicBlock::Create(module.getContext(), "if.else2");

    auto *elseCondBr = builder.CreateCondBr(compareResult, elseIfBB, elseElseBB);
    elseCondBr->removeFromParent();
    elseBB->getInstList().push_back(elseCondBr);

    // insert all new basic blocks into any_to_int new function
    newFunc->getBasicBlockList().push_back(entryBB);
    newFunc->getBasicBlockList().push_back(ifBB);
    newFunc->getBasicBlockList().push_back(elseBB);
    newFunc->getBasicBlockList().push_back(elseIfBB);
    newFunc->getBasicBlockList().push_back(elseElseBB);
    newFunc->getBasicBlockList().push_back(retBB);

    auto *constByteValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    llvm::BinaryOperator *addResult = llvm::BinaryOperator::CreateAdd(ptrToIntCast, constByteValue, "");
    elseIfBB->getInstList().push_back(addResult);

    auto *intToPtrCast = llvm::dyn_cast<llvm::Instruction>(
        builder.CreateIntToPtr(addResult, llvm::Type::getInt64PtrTy(module.getContext())));
    intToPtrCast->removeFromParent();
    elseIfBB->getInstList().push_back(intToPtrCast);

    llvm::StoreInst *storeResult = builder.CreateStore(intToPtrCast, localVarRef3);
    storeResult->removeFromParent();
    elseIfBB->getInstList().push_back(storeResult);

    llvm::LoadInst *locVarRefLoad = builder.CreateLoad(localVarRef3, "");
    locVarRefLoad->removeFromParent();
    elseIfBB->getInstList().push_back(locVarRefLoad);

    llvm::LoadInst *locVarRefLoadLoad = builder.CreateLoad(locVarRefLoad, "");
    locVarRefLoadLoad->removeFromParent();
    elseIfBB->getInstList().push_back(locVarRefLoadLoad);

    llvm::StoreInst *storeValueToIntValue = builder.CreateStore(locVarRefLoadLoad, localVarRef1);
    storeValueToIntValue->removeFromParent();
    elseIfBB->getInstList().push_back(storeValueToIntValue);

    llvm::Instruction *elseIfBBBrInsn = builder.CreateBr(retBB);
    elseIfBBBrInsn->removeFromParent();
    elseIfBB->getInstList().push_back(elseIfBBBrInsn);

    // abort call
    auto abortFunc = CodeGenUtils::getAbortFunc(module);
    auto *abortFuncCallInsn = builder.CreateCall(abortFunc);
    abortFuncCallInsn->removeFromParent();

    elseElseBB->getInstList().push_back(abortFuncCallInsn);

    llvm::Instruction *elseElseBBBrInsn = builder.CreateBr(retBB);
    elseElseBBBrInsn->removeFromParent();
    elseElseBB->getInstList().push_back(elseElseBBBrInsn);

    llvm::LoadInst *retValueRef = builder.CreateLoad(localVarRef1, "");
    retValueRef->removeFromParent();
    retBB->getInstList().push_back(retValueRef);

    llvm::Instruction *retValue = builder.CreateRet(retValueRef);
    retValue->removeFromParent();
    retBB->getInstList().push_back(retValue);
    return newFunc;
}

} // namespace nballerina
