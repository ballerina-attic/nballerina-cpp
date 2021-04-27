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

#include "BinaryOpInsn.h"
#include "Function.h"
#include "NonTerminatorInsnCodeGen.h"
#include "Operand.h"
#include "Package.h"
#include <string>

namespace nballerina {

void NonTerminatorInsnCodeGen::visit(class BinaryOpInsn &obj, llvm::IRBuilder<> &builder) {

    const std::string lhsTempName = obj.lhsOp.getName() + "_temp";
    auto *lhsRef = functionGenerator.getLocalOrGlobalVal(obj.lhsOp);
    auto *rhsOp1ref = functionGenerator.createTempVal(obj.rhsOp1, builder);
    auto *rhsOp2ref = functionGenerator.createTempVal(obj.rhsOp2, builder);
    TypeTag rhsType = obj.getFunctionRef().getLocalOrGlobalVariable(obj.rhsOp1).getType().getTypeTag();
    llvm::Value *binaryOpResult = nullptr;

    switch (obj.kind) {
    case INSTRUCTION_KIND_BINARY_ADD: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateAdd(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFAdd(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_SUB: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateSub(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFSub(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_MUL: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateMul(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFMul(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_DIV: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateUDiv(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFDiv(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_MOD: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateURem(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFRem(rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_THAN: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SGT, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGT, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_EQUAL: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SGE, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGE, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_THAN: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLT, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLT, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_EQUAL: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_SLE, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLE, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_EQUAL: {
        if (rhsType == nballerina::TYPE_TAG_INT || rhsType == nballerina::TYPE_TAG_NIL) {
            binaryOpResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OEQ, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_NOT_EQUAL: {
        if (rhsType == nballerina::TYPE_TAG_INT) {
            binaryOpResult = builder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else if (rhsType == nballerina::TYPE_TAG_FLOAT) {
            binaryOpResult = builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_ONE, rhsOp1ref, rhsOp2ref, lhsTempName);
        } else {
            llvm_unreachable("");
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_BITWISE_XOR: {
        binaryOpResult = builder.CreateXor(rhsOp1ref, rhsOp2ref, lhsTempName);
        break;
    }
    default:
        llvm_unreachable("");
    }
    builder.CreateStore(binaryOpResult, lhsRef);
}

} // namespace nballerina
