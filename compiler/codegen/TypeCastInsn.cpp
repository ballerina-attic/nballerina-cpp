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
#include "CodeGenUtils.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"

namespace nballerina {

TypeCastInsn::TypeCastInsn(const Operand &lhs, BasicBlock &currentBB, const Operand &rhsOp)
    : NonTerminatorInsn(lhs, currentBB), rhsOp(rhsOp) {}

void TypeCastInsn::translate(llvm::Module &module, llvm::IRBuilder<> &builder) {
    const auto &funcObj = getFunctionRef();
    auto *rhsOpRef = funcObj.getLLVMLocalOrGlobalVar(rhsOp, module);
    auto *lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand(), module);
    auto *lhsTypeRef = lhsOpRef->getType();

    const auto &lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    const auto &rhsVar = funcObj.getLocalOrGlobalVariable(rhsOp);
    const auto &lhsType = lhsVar.getType();
    auto lhsTypeTag = lhsType.getTypeTag();
    const auto &rhsType = rhsVar.getType();
    auto rhsTypeTag = rhsType.getTypeTag();

    if (Type::isSmartStructType(rhsTypeTag)) {
        if (Type::isSmartStructType(lhsTypeTag)) {
            auto *rhsVarOpRef = funcObj.createTempVariable(rhsOp, module, builder);
            builder.CreateStore(rhsVarOpRef, lhsOpRef);
            return;
        }
        // GEP of last type of smart pointer(original type of any variable(smart pointer))
        auto *inherentTypeIdx = builder.CreateStructGEP(rhsOpRef, 0, "inherentTypeName");
        auto *inherentTypeLoad = builder.CreateLoad(inherentTypeIdx);

        auto *data = builder.CreateStructGEP(rhsOpRef, 1, "data");
        auto *dataLoad = builder.CreateLoad(data);

        auto *strTblPtr = getPackageRef().getStringBuilderTableGlobalPointer();
        auto *strTblLoad = builder.CreateLoad(strTblPtr);
        auto *gepOfStr = builder.CreateInBoundsGEP(strTblLoad, llvm::ArrayRef<llvm::Value *>({inherentTypeLoad}));

        // get the mangled name of the lhs type and store it to string builder table.
        std::string_view lhsTypeName = Type::typeStringMangleName(lhsType);
        getPackageMutableRef().addToStrTable(lhsTypeName); // TODO : get rid of this mutable reference
        int tempRandNum = std::rand() % 1000 + 1;
        auto *constValue = builder.getInt64(tempRandNum);
        auto *lhsGep = builder.CreateInBoundsGEP(strTblLoad, llvm::ArrayRef<llvm::Value *>({constValue}));
        // call is_same_type rust function to check LHS and RHS type are same or not.
        auto isSameTypeFunc = CodeGenUtils::getIsSameTypeFunc(module, lhsGep, gepOfStr);
        auto *sameTypeResult = builder.CreateCall(isSameTypeFunc, llvm::ArrayRef<llvm::Value *>({lhsGep, gepOfStr}));
        // creating branch condition using is_same_type() function result.
        builder.CreateIsNotNull(sameTypeResult);
        getPackageMutableRef().addStringOffsetRelocationEntry(lhsTypeName.data(), lhsGep);

        auto *castResult = builder.CreateBitCast(dataLoad, lhsTypeRef, getLhsOperand().getName());
        auto *castLoad = builder.CreateLoad(castResult);
        builder.CreateStore(castLoad, lhsOpRef);
    } else if (Type::isSmartStructType(lhsTypeTag)) {
        getPackageMutableRef().storeValueInSmartStruct(module, builder, rhsOpRef, rhsType, lhsOpRef);
    } else if (lhsTypeTag == TYPE_TAG_INT && rhsTypeTag == TYPE_TAG_FLOAT) {
        auto *rhsLoad = builder.CreateLoad(rhsOpRef);
        auto *lhsLoad = builder.CreateLoad(lhsOpRef);
        auto *castResult = builder.CreateFPToSI(rhsLoad, lhsLoad->getType(), "");
        builder.CreateStore(castResult, lhsOpRef);
    } else {
        builder.CreateBitCast(rhsOpRef, lhsTypeRef, "data_cast");
    }
}

} // namespace nballerina
