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

#include "ConstantLoad.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace nballerina {

// With Nil Type setting only Type Tag because value will be zero with NIL Type.
ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB)
    : NonTerminatorInsn(lhs, std::move(currentBB)), typeTag(TYPE_TAG_NIL) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, int intVal)
    : NonTerminatorInsn(lhs, std::move(currentBB)), typeTag(TYPE_TAG_INT), intValue(intVal) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, float floatVal)
    : NonTerminatorInsn(lhs, std::move(currentBB)), typeTag(TYPE_TAG_FLOAT), floatValue(floatVal) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, bool boolVal)
    : NonTerminatorInsn(lhs, std::move(currentBB)), typeTag(TYPE_TAG_BOOLEAN), boolValue(boolVal) {}

ConstantLoadInsn::ConstantLoadInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, std::string str)
    : NonTerminatorInsn(lhs, std::move(currentBB)), typeTag(TYPE_TAG_STRING), strValue(std::move(str)) {}

void ConstantLoadInsn::translate(LLVMModuleRef &modRef) {
    LLVMValueRef constRef = nullptr;
    const auto &lhsOp = getLhsOperand();
    const auto &funcRef = getFunctionRef();
    LLVMBuilderRef builder = funcRef.getLLVMBuilder();
    LLVMValueRef lhsRef = funcRef.getLLVMLocalOrGlobalVar(lhsOp);

    assert(funcRef.getLocalOrGlobalVariable(lhsOp)->getType().getTypeTag() == typeTag);

    switch (typeTag) {
    case TYPE_TAG_INT: {
        constRef = LLVMConstInt(LLVMInt32Type(), intValue, 0);
        break;
    }
    case TYPE_TAG_FLOAT: {
        constRef = LLVMConstReal(LLVMFloatType(), floatValue);
        break;
    }
    case TYPE_TAG_BOOLEAN: {
        constRef = LLVMConstInt(LLVMInt8Type(), boolValue, 0);
        break;
    }
    case TYPE_TAG_STRING:
    case TYPE_TAG_CHAR_STRING: {
        std::string stringValue = strValue;
        llvm::Constant *C = llvm::ConstantDataArray::getString(
            *llvm::unwrap(LLVMGetGlobalContext()), llvm::StringRef(stringValue.c_str(), stringValue.length()));
        globalStringValue = std::make_unique<llvm::GlobalVariable>(*llvm::unwrap(modRef), C->getType(), false,
                                                                   llvm::GlobalValue::PrivateLinkage, C, ".str");
        globalStringValue->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        globalStringValue->setAlignment(llvm::Align(1));

        LLVMValueRef paramTypes[] = {llvm::wrap(llvm::unwrap(builder)->getInt64(0)),
                                     llvm::wrap(llvm::unwrap(builder)->getInt64(0))};
        constRef = LLVMBuildInBoundsGEP(builder, wrap(globalStringValue.get()), paramTypes, 2, "simple");
        break;
    }
    case TYPE_TAG_NIL: {
        const auto &lhsOpName = lhsOp.getName();
        // check for the main function and () is assigned to 0%
        assert(funcRef.getReturnVar().has_value());
        if (funcRef.isMainFunction() && (lhsOpName == funcRef.getReturnVar()->getName())) {
            return;
        }
        LLVMValueRef constTempRef = getPackageRef().getGlobalNilVar();
        std::string tempName = lhsOpName + "_temp";
        constRef = LLVMBuildLoad(builder, constTempRef, tempName.c_str());
        break;
    }
    default:
        llvm_unreachable("Unknown Type");
    }

    if ((constRef != nullptr) && (lhsRef != nullptr)) {
        LLVMBuildStore(builder, constRef, lhsRef);
    }
}

} // namespace nballerina
