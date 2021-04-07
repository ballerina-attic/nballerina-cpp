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

#include "ErrorTypeInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Types.h"
#include "Package.h"
#include "llvm-c/Core.h"

using namespace std;
using namespace llvm;

namespace nballerina {

ErrorTypeInsn::ErrorTypeInsn(const Operand &lOp, std::shared_ptr<BasicBlock> currentBB, const Operand &mOp, const Operand &cOp, const Operand &dOp, Type type)
    : NonTerminatorInsn(lOp, currentBB), msgOp(mOp), causeOp(cOp), detailOp(dOp), typeDecl(type) {}

const Operand &ErrorTypeInsn::getMsgOp() const { return msgOp; }
const Operand &ErrorTypeInsn::getCauseOp() const { return causeOp; }
const Operand &ErrorTypeInsn::getDetailOp() const { return detailOp; }
Type ErrorTypeInsn::getTypeDecl() { return typeDecl; }

LLVMValueRef ErrorTypeInsn::getNewString(LLVMModuleRef &modRef) {
    const char *newString = "new_string";
    LLVMValueRef addedStringRef = getPackageRef().getFunctionRef(newString);
    if (addedStringRef != nullptr) {
        return addedStringRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMPointerType(LLVMInt8Type(), 0), LLVMInt32Type()};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMPointerType(LLVMInt8Type(), 0), paramTypes, 2, 0);
    addedStringRef = LLVMAddFunction(modRef, newString, funcType);
    getPackageMutableRef().addFunctionRef(newString, addedStringRef);
    return addedStringRef;
}

void ErrorTypeInsn::translate(LLVMModuleRef &modRef) {
    LLVMValueRef addedStringRef = getNewString(modRef);
    // Fetch message string from Error Type Decl
    // Get opaque pointer to message string
    // Generate a function call to new_error() with above opaque pointer
}

} // namespace nballerina
