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
        return llvm::Type::getInt8PtrTy(context);
    case TYPE_TAG_ANY:
    case TYPE_TAG_UNION:
    case TYPE_TAG_ANYDATA: {
        auto *type = module.getTypeByName("struct.smtPtr");
        if (type != nullptr) {
            return type;
        }
        return llvm::StructType::create(
            context,
            llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt8PtrTy(context), llvm::Type::getInt8PtrTy(context)}),
            "struct.smtPtr");
    }
    case TYPE_TAG_TYPEDESC:
        return llvm::Type::getInt64Ty(context);
    default:
        llvm_unreachable("Invalid type");
    }
}

llvm::FunctionCallee CodeGenUtils::getMapStoreFunc(llvm::Module &module, TypeTag typeTag) {
    const std::string funcName = "map_store_" + Type::getNameOfType(typeTag);
    llvm::Type *memberType = Type::isSmartStructType(typeTag)
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
    llvm::Type *memType = Type::isSmartStructType(memberTypeTag)
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

    llvm::Type *funcRetType = Type::isSmartStructType(memberTypeTag)
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

// Splitting Basicblock after 'is_same_type()' function call and
// based on is_same_type() function result, crating branch condition using
// splitBB Basicblock (ifBB) and abortBB(elseBB).
// In IfBB we are doing casing and from ElseBB Aborting.
void CodeGenUtils::injectAbortCall(llvm::Module &module, llvm::IRBuilder<> &builder, const std::string &functionName) {
    const char *isSameTypeChar = "is_same_type";
    auto *llvmFunction = module.getFunction(functionName);
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
            lastInsn.eraseFromParent();
            // Creating abortBB (elseBB).
            llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(module.getContext(), "abortBB");

            // Creating Branch condition using if and else BB's.
            llvm::Instruction *compInsnRef = builder.CreateCondBr(compInsn, splitBB, elseBB);

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
            llvm::Instruction *brInsn = builder.CreateBr(elseBBSucc);
            brInsn->removeFromParent();

            // generate LLVMFunction call to Abort from elseLLVMBB(abortBB).
            auto abortFunc = CodeGenUtils::getAbortFunc(module);
            auto *abortFuncCallInsn = builder.CreateCall(abortFunc);
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
