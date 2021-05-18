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

#include "codegen/CodeGenUtils.h"

namespace nballerina {

llvm::Value *CodeGenUtils::getGlobalNilVar(llvm::Module &module) {
    static const std::string BAL_NIL_VALUE = "bal_nil_value";
    auto *nillGlobal = module.getGlobalVariable(BAL_NIL_VALUE, true);
    if (nillGlobal != nullptr) {
        return nillGlobal;
    }
    llvm::Type *nillType = CodeGenUtils::getLLVMTypeOfType(TYPE_TAG_NIL, module);
    return new llvm::GlobalVariable(module, nillType, false, llvm::GlobalValue::InternalLinkage,
                                    llvm::Constant::getNullValue(nillType), BAL_NIL_VALUE);
}

llvm::Type *CodeGenUtils::getLLVMTypeOfType(const Type &type, llvm::Module &module) {
    return getLLVMTypeOfType(type.getTypeTag(), module);
}

llvm::Type *CodeGenUtils::getLLVMTypeOfType(TypeTag typeTag, llvm::Module &module) {
    auto &context = module.getContext();
    switch (typeTag) {
    case TYPE_TAG_INT:
        return llvm::Type::getInt64Ty(context);
    case TYPE_TAG_FLOAT:
        return llvm::Type::getDoubleTy(context);
    case TYPE_TAG_BOOLEAN:
        return llvm::Type::getInt1Ty(context);
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING:
    case TYPE_TAG_MAP:
    case TYPE_TAG_ARRAY:
    case TYPE_TAG_NIL:
    case TYPE_TAG_ANY:
    case TYPE_TAG_ANYDATA:
    case TYPE_TAG_UNION:
        return llvm::Type::getInt8PtrTy(context);
    case TYPE_TAG_TYPEDESC:
        return llvm::Type::getInt64Ty(context);
    default:
        llvm_unreachable("Invalid type");
    }
}

llvm::FunctionCallee CodeGenUtils::getMapStoreFunc(llvm::Module &module, TypeTag typeTag) {
    const std::string funcName = "map_store_" + Type::getNameOfType(typeTag);
    llvm::Type *memberType = Type::isBalValueType(typeTag)
                                 ? llvm::PointerType::get(getLLVMTypeOfType(typeTag, module), 0)
                                 : getLLVMTypeOfType(typeTag, module);
    auto *keyType = getLLVMTypeOfType(TYPE_TAG_STRING, module);
    auto *mapType = getLLVMTypeOfType(TYPE_TAG_MAP, module);
    auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                             llvm::ArrayRef<llvm::Type *>({mapType, keyType, memberType}), false);
    return module.getOrInsertFunction(funcName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getStringInitFunc(llvm::Module &module) {
    const std::string newString = "new_string";
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt64Ty(module.getContext())}),
                                false);
    return module.getOrInsertFunction(newString, funcType);
}

llvm::FunctionCallee CodeGenUtils::getBoxValueFunc(llvm::Module &module, llvm::Type *paramType, TypeTag typeTag) {
    const std::string functionName = "box_bal_" + Type::getNameOfType(typeTag);
    auto *funcType =
        llvm::FunctionType::get(llvm::PointerType::get(paramType, 0), llvm::ArrayRef<llvm::Type *>({paramType}), false);
    return module.getOrInsertFunction(functionName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getAbortFunc(llvm::Module &module) {
    const std::string abortFuncName = "abort";
    auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()), false);
    return module.getOrInsertFunction(abortFuncName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getArrayInitFunc(llvm::Module &module, TypeTag memberTypeTag) {
    const auto arrayTypeFuncName = "array_init_" + Type::getNameOfType(memberTypeTag);
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(module.getContext())}), false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getArrayStoreFunc(llvm::Module &module, TypeTag memberTypeTag) {
    const auto arrayTypeFuncName = "array_store_" + Type::getNameOfType(memberTypeTag);
    llvm::Type *memType = Type::isBalValueType(memberTypeTag)
                              ? llvm::PointerType::get(CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module), 0)
                              : CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module);
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt64Ty(module.getContext()), memType}),
                                false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getArrayLoadFunc(llvm::Module &module, TypeTag memberTypeTag) {
    const auto arrayTypeFuncName = "array_load_" + Type::getNameOfType(memberTypeTag);

    llvm::Type *funcRetType = Type::isBalValueType(memberTypeTag)
                                  ? llvm::PointerType::get(CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module), 0)
                                  : CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module);
    auto *funcType =
        llvm::FunctionType::get(funcRetType,
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt64Ty(module.getContext())}),
                                false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getMapLoadFunc(llvm::Module &module, TypeTag memTypeTag) {
    auto *funcType = llvm::FunctionType::get(
        llvm::Type::getInt8Ty(module.getContext()),
        llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                      llvm::Type::getInt8PtrTy(module.getContext()),
                                      llvm::PointerType::get(CodeGenUtils::getLLVMTypeOfType(memTypeTag, module), 0)}),
        false);
    return module.getOrInsertFunction("map_load_" + Type::getNameOfType(memTypeTag), funcType);
}

llvm::FunctionCallee CodeGenUtils::getNewMapInitFunc(llvm::Module &module, TypeTag memTypeTag) {
    auto *funcType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module.getContext()), false);
    return module.getOrInsertFunction("map_new_" + Type::getNameOfType(memTypeTag), funcType);
}

llvm::FunctionCallee CodeGenUtils::getMapSpreadFieldInitFunc(llvm::Module &module) {
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(module.getContext()),
                                                              llvm::Type::getInt8PtrTy(module.getContext())}),
                                false);
    return module.getOrInsertFunction("map_spread_field_init", funcType);
}

llvm::FunctionCallee CodeGenUtils::getIsSameTypeFunc(llvm::Module &module, llvm::Value *lhsRef, llvm::Value *rhsRef) {
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getInt8PtrTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({lhsRef->getType(), rhsRef->getType()}), false);
    return module.getOrInsertFunction("is_same_type", funcType);
}

llvm::Function *CodeGenUtils::createIntToAnyFunction(llvm::Module &module, llvm::IRBuilder<> &builder,
                                                     llvm::BasicBlock *currBB) {

    const std::string functionName = "int_to_any";
    auto *newFunc = module.getFunction(functionName);
    if (newFunc != nullptr) {
        return newFunc;
    }
    // create new int_to_any function
    auto *funcType =
        llvm::FunctionType::get(builder.getInt8PtrTy(), llvm::ArrayRef<llvm::Type *>({builder.getInt64Ty()}), false);
    newFunc = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, functionName, module);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(module.getContext(), "entry", newFunc);
    builder.SetInsertPoint(entryBB);
    // create alloca of local variables in new function
    auto *localVarRef1 = builder.CreateAlloca(builder.getInt8PtrTy(), nullptr, "");
    auto *localVarRef2 = builder.CreateAlloca(builder.getInt64Ty(), nullptr, "");
    auto *localVarRef3 = builder.CreateAlloca(builder.getInt8PtrTy(), nullptr, "");
    auto *localVarRef4 = builder.CreateAlloca(llvm::Type::getInt64PtrTy(module.getContext()), nullptr, "");
    auto &funcArg = newFunc->arg_begin()[0];
    funcArg.setName("a");
    // creating instructions for entry BB in new function
    builder.CreateStore(&funcArg, localVarRef2);
    llvm::LoadInst *valueLoad = builder.CreateLoad(localVarRef2, "");
    auto *constIntValue = llvm::ConstantInt::get(builder.getInt64Ty(), 0x3FFFFFFFFFFFFFFF);
    auto *compWithPositiveVal = new llvm::ICmpInst(llvm::CmpInst::Predicate::ICMP_SGE, constIntValue, valueLoad, "");
    entryBB->getInstList().push_back(compWithPositiveVal);

    llvm::BasicBlock *ifBBEntry = llvm::BasicBlock::Create(module.getContext(), "lhs.true", newFunc);
    llvm::BasicBlock *ifBB = llvm::BasicBlock::Create(module.getContext(), "if.then", newFunc);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(module.getContext(), "if.else", newFunc);
    builder.CreateCondBr(compWithPositiveVal, ifBBEntry, elseBB);

    builder.SetInsertPoint(ifBBEntry);
    auto *constIntNegValue = llvm::ConstantInt::get(builder.getInt64Ty(), -0x4000000000000000);
    auto *compWithNegVal = new llvm::ICmpInst(llvm::CmpInst::Predicate::ICMP_SGE, valueLoad, constIntNegValue, "");
    ifBBEntry->getInstList().push_back(compWithNegVal);
    builder.CreateCondBr(compWithNegVal, ifBB, elseBB);

    builder.SetInsertPoint(ifBB);
    auto *constShlValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    llvm::BinaryOperator *shlInsn = llvm::BinaryOperator::CreateShl(valueLoad, constShlValue);
    shlInsn->setHasNoSignedWrap();
    llvm::BasicBlock *retBB = llvm::BasicBlock::Create(module.getContext(), "return", newFunc);
    llvm::BinaryOperator *orInsn = llvm::BinaryOperator::CreateOr(shlInsn, constShlValue);
    ifBB->getInstList().push_back(shlInsn);
    ifBB->getInstList().push_back(orInsn);
    auto *intToPtrCast = builder.CreateIntToPtr(orInsn, builder.getInt8PtrTy());
    builder.CreateStore(intToPtrCast, localVarRef1);
    // creating branch of the if basicblock.
    builder.CreateBr(retBB);

    builder.SetInsertPoint(elseBB);
    // generate elseBB instructions
    // here, adding balValueSize (8) and headerSize(1) "8 + 1 = 9" and loading that addition result.
    auto *addResult = llvm::ConstantInt::get(builder.getInt64Ty(), 8 + 1);
    llvm::Instruction *mallocInsn = llvm::CallInst::CreateMalloc(elseBB, builder.getInt64Ty(), builder.getInt8Ty(),
                                                                 addResult, nullptr, nullptr, "");
    elseBB->getInstList().push_back(mallocInsn);
    builder.CreateStore(mallocInsn, localVarRef3);
    llvm::LoadInst *mallocResLoad = builder.CreateLoad(localVarRef3, "");
    auto *headerSize = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    auto *gepOfMalloc =
        builder.CreateInBoundsGEP(builder.getInt8Ty(), mallocResLoad, llvm::ArrayRef<llvm::Value *>({headerSize}), "");
    auto *bitCastOfGEPMalloc = builder.CreateBitCast(gepOfMalloc, llvm::Type::getInt64PtrTy(module.getContext()), "");
    builder.CreateStore(bitCastOfGEPMalloc, localVarRef4);
    builder.CreateStore(builder.CreateLoad(localVarRef2, ""), builder.CreateLoad(localVarRef4, ""));
    builder.CreateStore(builder.CreateLoad(localVarRef3, ""), localVarRef1);
    builder.CreateBr(retBB);

    builder.SetInsertPoint(retBB);
    builder.CreateRet(builder.CreateLoad(localVarRef1, ""));

    builder.SetInsertPoint(currBB);
    return newFunc;
}

llvm::Function *CodeGenUtils::createAnyToIntFunction(llvm::Module &module, llvm::IRBuilder<> &builder,
                                                     llvm::BasicBlock *currBB) {

    const std::string functionName = "any_to_int";
    auto *newFunc = module.getFunction(functionName);
    if (newFunc != nullptr) {
        return newFunc;
    }
    // create new any_to_int function
    auto *funcType =
        llvm::FunctionType::get(builder.getInt64Ty(), llvm::ArrayRef<llvm::Type *>({builder.getInt8PtrTy()}), false);
    newFunc = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, functionName, module);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(module.getContext(), "entry", newFunc);
    builder.SetInsertPoint(entryBB);
    // create alloca of local variables
    auto *localVarRef1 = builder.CreateAlloca(builder.getInt64Ty(), nullptr, "");
    auto *localVarRef2 = builder.CreateAlloca(builder.getInt8PtrTy(), nullptr, "");
    auto *localVarRef3 = builder.CreateAlloca(llvm::Type::getInt64PtrTy(module.getContext()), nullptr, "");
    auto &funcArg = newFunc->arg_begin()[0];
    funcArg.setName("a");

    builder.CreateStore(&funcArg, localVarRef2);
    llvm::LoadInst *valueLoad = builder.CreateLoad(localVarRef2, "");
    auto *ptrToIntCast = builder.CreatePtrToInt(valueLoad, builder.getInt64Ty(), "");
    auto *constOneValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    llvm::Instruction *andInsn = llvm::BinaryOperator::CreateAnd(ptrToIntCast, constOneValue);
    entryBB->getInstList().push_back(andInsn);
    auto *constZeroValue = llvm::ConstantInt::get(builder.getInt64Ty(), 0);
    llvm::Instruction *ifReturn = new llvm::ICmpInst(llvm::CmpInst::Predicate::ICMP_NE, andInsn, constZeroValue, "");
    entryBB->getInstList().push_back(ifReturn);

    // create if BB
    llvm::BasicBlock *ifBB = llvm::BasicBlock::Create(module.getContext(), "if.then", newFunc);
    // create else BB
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(module.getContext(), "if.else", newFunc);
    // Creating Branch condition using if and else BB's.
    builder.CreateCondBr(ifReturn, ifBB, elseBB);

    builder.SetInsertPoint(ifBB);
    llvm::BinaryOperator *shrInsn = llvm::BinaryOperator::CreateAShr(ptrToIntCast, constOneValue);
    ifBB->getInstList().push_back(shrInsn);
    llvm::StoreInst *storeValueToBalValue = builder.CreateStore(shrInsn, localVarRef1);
    ifBB->getInstList().push_back(storeValueToBalValue);
    // create if BB in else BB
    llvm::BasicBlock *elseIfBB = llvm::BasicBlock::Create(module.getContext(), "if.then2", newFunc);
    // create else BB in else BB
    llvm::BasicBlock *elseElseBB = llvm::BasicBlock::Create(module.getContext(), "if.else2", newFunc);
    // create return BB
    llvm::BasicBlock *retBB = llvm::BasicBlock::Create(module.getContext(), "return", newFunc);
    // creating branch of the if basicblock.
    builder.CreateBr(retBB);

    builder.SetInsertPoint(elseBB);
    auto *constTagMask = llvm::ConstantInt::get(builder.getInt64Ty(), 0b11);
    llvm::Instruction *andInsnElseBB = llvm::BinaryOperator::CreateAnd(ptrToIntCast, constTagMask);
    elseBB->getInstList().push_back(andInsnElseBB);
    auto *compareResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, andInsnElseBB, constZeroValue, "");
    builder.CreateCondBr(compareResult, elseIfBB, elseElseBB);

    builder.SetInsertPoint(elseIfBB);
    auto *constByteValue = llvm::ConstantInt::get(builder.getInt64Ty(), 1);
    llvm::BinaryOperator *addResult = llvm::BinaryOperator::CreateAdd(ptrToIntCast, constByteValue, "");
    elseIfBB->getInstList().push_back(addResult);
    auto *intToPtrCast = builder.CreateIntToPtr(addResult, llvm::Type::getInt64PtrTy(module.getContext()));
    builder.CreateStore(intToPtrCast, localVarRef3);
    llvm::LoadInst *locVarRefLoad = builder.CreateLoad(builder.CreateLoad(localVarRef3, ""), "");
    builder.CreateStore(locVarRefLoad, localVarRef1);
    builder.CreateBr(retBB);

    // abort call
    builder.SetInsertPoint(elseElseBB);
    auto *abortCall = builder.CreateCall(getAbortFunc(module));
    abortCall->addAttribute(~0U, llvm::Attribute::NoReturn);
    abortCall->addAttribute(~0U, llvm::Attribute::NoUnwind);
    builder.CreateUnreachable();

    builder.SetInsertPoint(retBB);
    builder.CreateRet(builder.CreateLoad(localVarRef1, ""));

    builder.SetInsertPoint(currBB);
    return newFunc;
}

} // namespace nballerina
