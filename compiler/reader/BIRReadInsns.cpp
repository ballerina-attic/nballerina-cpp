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

#include "reader/BIRReadInsns.h"
#include "bir/ArrayInstructions.h"
#include "bir/BinaryOpInsn.h"
#include "bir/ConditionBrInsn.h"
#include "bir/ConstantLoad.h"
#include "bir/FunctionCallInsn.h"
#include "bir/FunctionParam.h"
#include "bir/GoToInsn.h"
#include "bir/InvocableType.h"
#include "bir/MapInsns.h"
#include "bir/MoveInsn.h"
#include "bir/ReturnInsn.h"
#include "bir/StructureInsn.h"
#include "bir/TypeCastInsn.h"
#include "bir/TypeDescInsn.h"
#include "bir/TypeTestInsn.h"
#include "bir/UnaryOpInsn.h"
#include <iostream>

namespace nballerina {

// Read Local Variable and return Variable pointer
static Operand readOperand(Parser &reader, ConstantPoolSet &cp) {
    [[maybe_unused]] uint8_t ignoredVar = reader.readU1();

    uint8_t kind = reader.readU1();

    [[maybe_unused]] uint8_t scope = reader.readU1();

    int32_t varDclNameCpIndex = reader.readS4be();

    if ((VarKind)kind == GLOBAL_VAR_KIND) {
        [[maybe_unused]] int32_t packageIndex = reader.readS4be();
        int32_t typeCpIndex = reader.readS4be();
        [[maybe_unused]] Type typeDecl = cp.getTypeCp(typeCpIndex, false);
    }

    return Operand(cp.getStringCp(varDclNameCpIndex), (VarKind)kind);
}

void BIRReadInsn::readInsn(BasicBlock &basicBlock, Parser &reader, ConstantPoolSet &cp) {
    int32_t sourceFileCpIndex = reader.readS4be();
    int32_t sLine = reader.readS4be();
    int32_t sCol = reader.readS4be();
    int32_t eLine = reader.readS4be();
    int32_t eCol = reader.readS4be();
    [[maybe_unused]] Location location(cp.getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    auto insnKind = (InstructionKind)reader.readU1();
    switch (insnKind) {
    case INSTRUCTION_KIND_NEW_TYPEDESC: {
        ReadTypeDescInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_NEW_STRUCTURE: {
        ReadStructureInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_CONST_LOAD: {
        ReadConstLoadInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_GOTO: {
        ReadGoToInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_RETURN: {
        ReadReturnInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_BINARY_ADD:
    case INSTRUCTION_KIND_BINARY_SUB:
    case INSTRUCTION_KIND_BINARY_MUL:
    case INSTRUCTION_KIND_BINARY_DIV:
    case INSTRUCTION_KIND_BINARY_EQUAL:
    case INSTRUCTION_KIND_BINARY_NOT_EQUAL:
    case INSTRUCTION_KIND_BINARY_GREATER_THAN:
    case INSTRUCTION_KIND_BINARY_GREATER_EQUAL:
    case INSTRUCTION_KIND_BINARY_LESS_THAN:
    case INSTRUCTION_KIND_BINARY_LESS_EQUAL:
    case INSTRUCTION_KIND_BINARY_BITWISE_XOR:
    case INSTRUCTION_KIND_BINARY_MOD: {
        ReadBinaryInsn(basicBlock, insnKind, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_UNARY_NEG:
    case INSTRUCTION_KIND_UNARY_NOT: {
        ReadUnaryInsn(basicBlock, insnKind, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
        ReadCondBrInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_MOVE: {
        ReadMoveInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_CALL: {
        ReadFuncCallInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_TYPE_CAST: {
        ReadTypeCastInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_TYPE_TEST: {
        ReadTypeTestInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_NEW_ARRAY: {
        ReadArrayInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_ARRAY_STORE: {
        ReadArrayStoreInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_ARRAY_LOAD: {
        ReadArrayLoadInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_MAP_STORE: {
        ReadMapStoreInsn(basicBlock, reader, cp);
        break;
    }
    case INSTRUCTION_KIND_MAP_LOAD: {
        ReadMapLoadInsn(basicBlock, reader, cp);
        break;
    }
    default:
        std::cerr << "Unsupported Instruction type: " << insnKind << std::endl;
        abort();
    }
}

// Read Mapping Constructor Key Value body
static MapConstruct readMapConstructor(Parser &reader, ConstantPoolSet &cp) {

    auto kind = reader.readU1();
    if ((MapConstrctBodyKind)kind == Spread_Field_Kind) {
        auto expr = readOperand(reader, cp);
        return MapConstruct(MapConstruct::SpreadField(std::move(expr)));
    }
    // For Key_Value_Kind
    auto key = readOperand(reader, cp);
    auto value = readOperand(reader, cp);
    return MapConstruct(MapConstruct::KeyValue(std::move(key), std::move(value)));
}

// Read TYPEDESC Insn
void BIRReadInsn::ReadTypeDescInsn(BasicBlock &, Parser &reader, ConstantPoolSet &cp) {
    [[maybe_unused]] auto lhsOp = readOperand(reader, cp);
    [[maybe_unused]] int32_t typeCpIndex = reader.readS4be();
}

// Read STRUCTURE Insn
void BIRReadInsn::ReadStructureInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto rhsOp = readOperand(reader, cp);
    [[maybe_unused]] auto lhsOp = readOperand(reader, cp);

    auto initValuesCount = reader.readS4be();

    if (initValuesCount == 0) {
        currentBB.addNonTermInsn(std::make_unique<StructureInsn>(std::move(lhsOp), currentBB));
        return;
    }

    std::vector<MapConstruct> initValues;
    initValues.reserve(initValuesCount);
    for (auto i = 0; i < initValuesCount; i++) {
        initValues.push_back(readMapConstructor(reader, cp));
    }
    currentBB.addNonTermInsn(std::make_unique<StructureInsn>(std::move(lhsOp), currentBB, std::move(initValues)));
}

// Read CONST_LOAD Insn
void BIRReadInsn::ReadConstLoadInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    [[maybe_unused]] int32_t typeCpIndex = reader.readS4be();
    auto lhsOp = readOperand(reader, cp);

    switch (cp.getTypeTag(typeCpIndex)) {
    case TYPE_TAG_INT:
    case TYPE_TAG_UNSIGNED8_INT:
    case TYPE_TAG_UNSIGNED16_INT:
    case TYPE_TAG_UNSIGNED32_INT:
    case TYPE_TAG_SIGNED8_INT:
    case TYPE_TAG_SIGNED16_INT:
    case TYPE_TAG_SIGNED32_INT:
    case TYPE_TAG_DECIMAL: {
        int32_t valueCpIndex = reader.readS4be();
        currentBB.addNonTermInsn(
            ConstantLoadInsn::createIntConstLoad(std::move(lhsOp), currentBB, (int64_t)cp.getIntCp(valueCpIndex)));
        return;
    }

    case TYPE_TAG_BYTE: {
        int32_t valueCpIndex = reader.readS4be();
        currentBB.addNonTermInsn(
            ConstantLoadInsn::createByteConstLoad(std::move(lhsOp), currentBB, (int8_t)cp.getByteCp(valueCpIndex)));
        return;
    }

    case TYPE_TAG_BOOLEAN: {
        uint8_t boolean_constant = reader.readU1();
        if (boolean_constant == 0) {
            currentBB.addNonTermInsn(ConstantLoadInsn::createBoolConstLoad(std::move(lhsOp), currentBB, false));
            return;
        }
        currentBB.addNonTermInsn(ConstantLoadInsn::createBoolConstLoad(std::move(lhsOp), currentBB, true));
        return;
    }
    case TYPE_TAG_FLOAT: {
        int32_t valueCpIndex = reader.readS4be();
        currentBB.addNonTermInsn(
            ConstantLoadInsn::createFloatConstLoad(std::move(lhsOp), currentBB, cp.getFloatCp(valueCpIndex)));
        return;
    }
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING: {
        int32_t valueCpIndex = reader.readS4be();
        currentBB.addNonTermInsn(
            ConstantLoadInsn::createStringConstLoad(std::move(lhsOp), currentBB, cp.getStringCp(valueCpIndex)));
        return;
    }
    case TYPE_TAG_NIL: {
        currentBB.addNonTermInsn(ConstantLoadInsn::createNullConstLoad(std::move(lhsOp), currentBB));
        return;
    }
    default: {
        std::cerr << "Unsupported type tag" << std::endl;
        abort();
    }
    }
}

// Read Unary Operand
void BIRReadInsn::ReadUnaryInsn(BasicBlock &currentBB, InstructionKind kind, Parser &reader, ConstantPoolSet &cp) {
    auto rhsOp = readOperand(reader, cp);
    auto lhsOp = readOperand(reader, cp);
    currentBB.addNonTermInsn(std::make_unique<UnaryOpInsn>(std::move(lhsOp), currentBB, std::move(rhsOp), kind));
}

// Read Binary Operand
void BIRReadInsn::ReadBinaryInsn(BasicBlock &currentBB, InstructionKind kind, Parser &reader, ConstantPoolSet &cp) {
    auto rhsOp1 = readOperand(reader, cp);
    auto rhsOp2 = readOperand(reader, cp);
    auto lhsOp = readOperand(reader, cp);
    currentBB.addNonTermInsn(
        std::make_unique<BinaryOpInsn>(std::move(lhsOp), currentBB, std::move(rhsOp1), std::move(rhsOp2), kind));
}

// Read BRANCH Insn
void BIRReadInsn::ReadCondBrInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto lhsOp = readOperand(reader, cp);
    int32_t trueBbIdNameCpIndex = reader.readS4be();
    int32_t falseBbIdNameCpIndex = reader.readS4be();

    currentBB.setTerminatorInsn(std::make_unique<ConditionBrInsn>(
        std::move(lhsOp), currentBB, cp.getStringCp(trueBbIdNameCpIndex), cp.getStringCp(falseBbIdNameCpIndex)));
}

// Read MOV Insn
void BIRReadInsn::ReadMoveInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto rhsOp = readOperand(reader, cp);
    auto lhsOp = readOperand(reader, cp);
    currentBB.addNonTermInsn(std::make_unique<MoveInsn>(std::move(lhsOp), currentBB, std::move(rhsOp)));
}

// Read Function Call
void BIRReadInsn::ReadFuncCallInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    [[maybe_unused]] uint8_t isVirtual = reader.readU1();
    [[maybe_unused]] int32_t packageIndex = reader.readS4be();
    int32_t callNameCpIndex = reader.readS4be();
    std::string funcName = cp.getStringCp(callNameCpIndex);
    int32_t argumentsCount = reader.readS4be();

    std::vector<Operand> fnArgs;
    fnArgs.reserve(argumentsCount);
    for (auto i = 0; i < argumentsCount; i++) {
        fnArgs.push_back(readOperand(reader, cp));
    }

    uint8_t hasLhsOperand = reader.readU1();
    Operand lhsOp = (hasLhsOperand > 0) ? readOperand(reader, cp) : Operand("", NOT_A_KIND);

    auto thenBbIdNameCpIndex = reader.readS4be();

    currentBB.setTerminatorInsn(std::make_unique<FunctionCallInsn>(currentBB, cp.getStringCp(thenBbIdNameCpIndex),
                                                                   std::move(lhsOp), std::move(funcName),
                                                                   argumentsCount, std::move(fnArgs)));
}

// Read TypeCast Insn
void BIRReadInsn::ReadTypeCastInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto lhsOp = readOperand(reader, cp);
    auto rhsOperand = readOperand(reader, cp);

    int32_t typeCpIndex = reader.readS4be();
    Type typeDecl = cp.getTypeCp(typeCpIndex, false);
    [[maybe_unused]] uint8_t isCheckTypes = reader.readU1();

    currentBB.addNonTermInsn(std::make_unique<TypeCastInsn>(std::move(lhsOp), currentBB, std::move(rhsOperand)));
}

// Read Type Test Insn
void BIRReadInsn::ReadTypeTestInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    int32_t typeCpIndex = reader.readS4be();
    Type typeDecl = cp.getTypeCp(typeCpIndex, false);
    auto lhsOp = readOperand(reader, cp);
    [[maybe_unused]] auto rhsOperand = readOperand(reader, cp);
    currentBB.addNonTermInsn(std::make_unique<TypeTestInsn>(std::move(lhsOp), currentBB));
}

// Read Array Insn
void BIRReadInsn::ReadArrayInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    int32_t typeCpIndex = reader.readS4be();
    Type typeDecl = cp.getTypeCp(typeCpIndex, false);
    auto lhsOp = readOperand(reader, cp);
    auto sizeOperand = readOperand(reader, cp);

    // TODO handle Array init values
    auto init_values_count = reader.readS4be();
    for (auto i = 0; i < init_values_count; i++) {
        [[maybe_unused]] auto init_value = readOperand(reader, cp);
    }
    currentBB.addNonTermInsn(std::make_unique<ArrayInsn>(std::move(lhsOp), currentBB, std::move(sizeOperand)));
}

// Read Array Store Insn
void BIRReadInsn::ReadArrayStoreInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto lhsOp = readOperand(reader, cp);
    auto keyOperand = readOperand(reader, cp);
    auto rhsOperand = readOperand(reader, cp);
    currentBB.addNonTermInsn(
        std::make_unique<ArrayStoreInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand)));
}

// Read Array Load Insn
void BIRReadInsn::ReadArrayLoadInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    [[maybe_unused]] uint8_t optionalFieldAccess = reader.readU1();
    [[maybe_unused]] uint8_t fillingRead = reader.readU1();
    auto lhsOp = readOperand(reader, cp);
    auto keyOperand = readOperand(reader, cp);
    auto rhsOperand = readOperand(reader, cp);
    currentBB.addNonTermInsn(
        std::make_unique<ArrayLoadInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand)));
}

// Read Map Store Insn
void BIRReadInsn::ReadMapStoreInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto lhsOp = readOperand(reader, cp);
    auto keyOperand = readOperand(reader, cp);
    auto rhsOperand = readOperand(reader, cp);
    currentBB.addNonTermInsn(
        std::make_unique<MapStoreInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand)));
}

// Read Map Load Insn
void BIRReadInsn::ReadMapLoadInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    [[maybe_unused]] uint8_t optionalFieldAccess = reader.readU1();
    [[maybe_unused]] uint8_t fillingRead = reader.readU1();
    auto lhsOp = readOperand(reader, cp);
    auto keyOperand = readOperand(reader, cp);
    auto rhsOperand = readOperand(reader, cp);
    currentBB.addNonTermInsn(
        std::make_unique<MapLoadInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand)));
}

void BIRReadInsn::ReadGoToInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp) {
    auto nameId = reader.readS4be();
    currentBB.setTerminatorInsn(std::make_unique<GoToInsn>(currentBB, cp.getStringCp(nameId)));
}

void BIRReadInsn::ReadReturnInsn(BasicBlock &currentBB, Parser &, ConstantPoolSet &) {
    currentBB.setTerminatorInsn(std::make_unique<ReturnInsn>(currentBB));
}

} // namespace nballerina
