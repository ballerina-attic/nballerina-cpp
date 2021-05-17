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
#include "codegen/config.h"
#include <iostream>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Transforms/Utils/Cloning.h>

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
    case TYPE_TAG_ARRAY: {
        // TODO: remove this once other array types are supported
        /*
           Runtime implements this type only for int arrays. Other arrays are still of the type Int8PtrTy.
           Though LLVM ir generated assume all array types to be this type it will be silently ignored by the
           runtime.
        */
        auto *dynamicBalArrayType = module.getTypeByName("struct.dynamicBalArray");

        if (dynamicBalArrayType != nullptr) {
            return llvm::PointerType::getUnqual(dynamicBalArrayType);
        }

        assert(module.getTypeByName("struct.dynamicArray") == nullptr);

        auto *dynamicArrayType = llvm::StructType::create(
            context,
            llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(context), llvm::Type::getInt64PtrTy(context)}),
            "struct.dynamicArray");
        dynamicBalArrayType = llvm::StructType::create(
            context,
            llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context),
                                          llvm::Type::getInt64Ty(context), llvm::Type::getInt64Ty(context),
                                          llvm::PointerType::getUnqual(dynamicArrayType)}),
            "struct.dynamicBalArray");
        return llvm::PointerType::getUnqual(dynamicBalArrayType);
    }
    case TYPE_TAG_TYPEDESC:
        return llvm::Type::getInt64Ty(context);
    default:
        llvm_unreachable("Invalid type");
    }
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
    // TODO: remove this once other array types are supported
    /*
        Runtime implements this type only for int arrays. Other arrays are still of the type Int8PtrTy.
        This difference is silently ignored by the runtime.
    */
    auto *funcType =
        llvm::FunctionType::get(CodeGenUtils::getLLVMTypeOfType(TYPE_TAG_ARRAY, module),
                                llvm::ArrayRef<llvm::Type *>({llvm::Type::getInt64Ty(module.getContext())}), false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getArrayStoreFunc(llvm::Module &module, TypeTag memberTypeTag) {
    const auto arrayTypeFuncName = "array_store_" + Type::getNameOfType(memberTypeTag);
    // TODO: remove this once other array types are supported
    /*
        Runtime implements this type only for int arrays. Other arrays are still of the type Int8PtrTy.
        This difference is silently ignored by the runtime.
    */
    llvm::Type *memType = Type::isSmartStructType(memberTypeTag)
                              ? llvm::PointerType::get(CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module), 0)
                              : CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module);
    auto *funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                llvm::ArrayRef<llvm::Type *>({CodeGenUtils::getLLVMTypeOfType(TYPE_TAG_ARRAY, module),
                                                              llvm::Type::getInt64Ty(module.getContext()), memType}),
                                false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
}

llvm::FunctionCallee CodeGenUtils::getArrayLoadFunc(llvm::Module &module, TypeTag memberTypeTag) {
    const auto arrayTypeFuncName = "array_load_" + Type::getNameOfType(memberTypeTag);

    llvm::Type *funcRetType = Type::isSmartStructType(memberTypeTag)
                                  ? llvm::PointerType::get(CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module), 0)
                                  : CodeGenUtils::getLLVMTypeOfType(memberTypeTag, module);
    // TODO: remove this once other array types are supported
    /*
        Runtime implements this type only for int arrays. Other arrays are still of the type Int8PtrTy.
        This difference is silently ignored by the runtime.
    */
    auto *funcType =
        llvm::FunctionType::get(funcRetType,
                                llvm::ArrayRef<llvm::Type *>({CodeGenUtils::getLLVMTypeOfType(TYPE_TAG_ARRAY, module),
                                                              llvm::Type::getInt64Ty(module.getContext())}),
                                false);
    return module.getOrInsertFunction(arrayTypeFuncName, funcType);
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

std::unique_ptr<llvm::Module> CodeGenUtils::parseLLFile(llvm::LLVMContext &mContext, const std::string &fileName) {
    llvm::SMDiagnostic Err;
    std::unique_ptr<llvm::Module> srcModule = llvm::parseIRFile(LL_FILE_PATH + fileName, Err, mContext);
    if (!srcModule) {
        std::cerr << Err.getMessage().str() << std::endl;
        abort();
    }

    return srcModule;
}

llvm::FunctionCallee CodeGenUtils::replaceProtoFunc(const std::string &funcName, llvm::Module &destModule,
                                                    llvm::Module &srcModule) {
    for (const auto &func : srcModule.getFunctionList()) {
        destModule.getOrInsertFunction(func.getName(), func.getFunctionType());
    }
    llvm::Function *destFunc = destModule.getFunction(funcName);
    assert(destFunc!=nullptr);
    llvm::Function *srcFunc = srcModule.getFunction(funcName);
    assert(srcFunc!=nullptr);
    llvm::ValueToValueMapTy valuemap;
    for (auto srcArgs = srcFunc->arg_begin(), destArgs = destFunc->arg_begin(); srcArgs != srcFunc->arg_end();
         ++destArgs, ++srcArgs) {
        valuemap[srcArgs] = destArgs;
    }
    llvm::SmallVector<llvm::ReturnInst *, 0> returns;
    llvm::CloneFunctionInto(destFunc, srcFunc, valuemap, false, returns);
    return destFunc;
}
llvm::FunctionCallee CodeGenUtils::getNewMapInitFunc(llvm::Module &module) {
    auto *funcType = llvm::FunctionType::get(getLLVMTypeOfType(TYPE_TAG_MAP, module), false);
    return module.getOrInsertFunction("bal_map_create", funcType);
}

llvm::FunctionCallee CodeGenUtils::getMapLoadFunc(llvm::Module &module) {
    auto *funcType = llvm::FunctionType::get(
        llvm::Type::getInt8Ty(module.getContext()),
        llvm::ArrayRef<llvm::Type *>({getLLVMTypeOfType(TYPE_TAG_MAP, module),
                                      getLLVMTypeOfType(TYPE_TAG_STRING, module),
                                      llvm::PointerType::get(getLLVMTypeOfType(TYPE_TAG_INT, module), 0)}),
        false);
    return module.getOrInsertFunction("bal_map_lookup", funcType);
}

llvm::FunctionCallee CodeGenUtils::getMapStoreFunc(llvm::Module &module) {
    llvm::Type *memberType = getLLVMTypeOfType(TYPE_TAG_INT, module);
    auto *keyType = getLLVMTypeOfType(TYPE_TAG_STRING, module);
    auto *mapType = getLLVMTypeOfType(TYPE_TAG_MAP, module);
    auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(module.getContext()),
                                             llvm::ArrayRef<llvm::Type *>({mapType, keyType, memberType}), false);
    return module.getOrInsertFunction("bal_map_insert", funcType);
}

} // namespace nballerina
