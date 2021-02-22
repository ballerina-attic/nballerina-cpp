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

#include "TypeCastInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"

using namespace std;
using namespace llvm;

namespace nballerina {

TypeCastInsn::TypeCastInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp, [[maybe_unused]] Type *tDecl,
                           [[maybe_unused]] bool checkTypes)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp) {}

void TypeCastInsn::translate([[maybe_unused]] LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    string lhsOpName = getLHS()->getName();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLHS());
    LLVMTypeRef lhsTypeRef = wrap(unwrap(lhsOpRef)->getType());

    TypeTag rhsTypeTag = funcObj->getLocalOrGlobalVariable(getLHS())->getTypeDecl()->getTypeTag();
    TypeTag lhsTypeTag = funcObj->getLocalOrGlobalVariable(rhsOp)->getTypeDecl()->getTypeTag();
    if (rhsTypeTag == TYPE_TAG_ANY || rhsTypeTag == TYPE_TAG_UNION) {
        LLVMValueRef lastTypeIdx __attribute__((unused)) = LLVMBuildStructGEP(builder, rhsOpRef, 0, "inherentTypeIdx");
        if (lhsTypeTag == TYPE_TAG_UNION || lhsTypeTag == TYPE_TAG_ANY) {
            LLVMValueRef rhsVarOpRef = getFunction()->getTempLocalVariable(rhsOp);
            LLVMBuildStore(builder, rhsVarOpRef, lhsOpRef);
            return;
        }
        LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 1, "data");
        LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");
        LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, lhsOpName.c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
        LLVMBuildStore(builder, castLoad, lhsOpRef);
    } else if (lhsTypeTag == TYPE_TAG_UNION || lhsTypeTag == TYPE_TAG_ANY) {
        StringTableBuilder *strTable = getPackage()->getStrTableBuilder();

        LLVMValueRef inherentTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 0, "inherentTypeIdx");
        // TypeTagEnum rhsTypeTagEnum = TypeTagEnum(rhsTypeTag);
        const char *rhsTypeName = Type::getNameOfType(rhsTypeTag);
        if (!strTable->contains(rhsTypeName))
            strTable->add(rhsTypeName);
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), -1, 0);
        LLVMValueRef lhsTypeStoreRef = LLVMBuildStore(builder, constValue, inherentTypeIdx);
        getPackage()->addStringOffsetRelocationEntry(rhsTypeName, lhsTypeStoreRef);
        LLVMValueRef data = LLVMBuildStructGEP(builder, lhsOpRef, 1, "data");
        LLVMValueRef bitCast = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMBuildStore(builder, bitCast, data);
    } else {
        LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
}

} // namespace nballerina
