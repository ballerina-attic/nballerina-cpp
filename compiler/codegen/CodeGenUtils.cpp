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

std::unique_ptr<llvm::Module> CodeGenUtils::parseLLFile(llvm::LLVMContext &mContext){
    llvm::SMDiagnostic Err;
    //reading fun.ll for testing
    std::unique_ptr<llvm::Module> srcModule = llvm::parseIRFile(LLFilePath, Err, mContext);
	if (!srcModule)
	{
		Err.print("Open Module file error", llvm::errs());
		return NULL;
	}
	llvm::Module* M = srcModule.get();
	if (!M)
	{
		llvm::errs() << ": error loading file \n";
		return NULL;
	}
    return srcModule;
}

llvm::FunctionCallee CodeGenUtils::replaceProtoFunc(std::string funcName, llvm::Module &destModule, llvm::Module* srcModule ){
    auto destFunc = destModule.getFunction(funcName);
    if (destFunc != NULL){
        return destFunc;
    }
    llvm::Function *srcFunc = srcModule->getFunction(funcName);
    llvm::FunctionType *funcType = srcFunc->getFunctionType();
    destFunc = llvm::Function::Create(funcType, srcFunc->getLinkage(), funcName, &destModule);
    for (auto curFunc = srcModule->getFunctionList().begin(), 
            endFunc = srcModule->getFunctionList().end(); 
            curFunc != endFunc; ++curFunc) {
            destModule.getOrInsertFunction(curFunc->getName(), curFunc->getFunctionType());
    }
    llvm::ValueToValueMapTy valuemap;
    auto destArgs = destFunc->arg_begin();
    auto srcArgs = srcFunc->arg_begin();
    for (auto FArgsEnd = srcFunc->arg_end(); srcArgs != FArgsEnd; ++destArgs, ++srcArgs) {
        valuemap[&*srcArgs] = &*destArgs;
    }
    llvm::SmallVector<llvm::ReturnInst*, 8> returns;
    llvm::CloneFunctionInto(destFunc, srcFunc , valuemap, false, returns);
    return destModule.getFunction(funcName);
}
} // namespace nballerina
