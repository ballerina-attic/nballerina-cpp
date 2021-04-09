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

#include "BIRReader.h"
#include <array>
#include <cstdio>
#include <cstdlib>

#ifdef unix
#include <libgen.h>
#else
#define __attribute__(unused)
#endif

using namespace std;
using namespace nballerina;

// Instruction readers object declarations
ReadCondBrInsn ReadCondBrInsn::readCondBrInsn;
ReadFuncCallInsn ReadFuncCallInsn::readFuncCallInsn;
ReadGoToInsn ReadGoToInsn::readGoToInsn;
ReadReturnInsn ReadReturnInsn::readReturnInsn;
ReadBinaryInsn ReadBinaryInsn::readBinaryInsn;
ReadUnaryInsn ReadUnaryInsn::readUnaryInsn;
ReadConstLoadInsn ReadConstLoadInsn::readConstLoadInsn;
ReadMoveInsn ReadMoveInsn::readMoveInsn;
ReadTypeDescInsn ReadTypeDescInsn::readTypeDescInsn;
ReadStructureInsn ReadStructureInsn::readStructureInsn;
ReadTypeCastInsn ReadTypeCastInsn::readTypeCastInsn;
ReadTypeTestInsn ReadTypeTestInsn::readTypeTestInsn;
ReadArrayInsn ReadArrayInsn::readArrayInsn;
ReadArrayStoreInsn ReadArrayStoreInsn::readArrayStoreInsn;
ReadArrayLoadInsn ReadArrayLoadInsn::readArrayLoadInsn;
ReadMapStoreInsn ReadMapStoreInsn::readMapStoreInsn;
ReadMapLoadInsn ReadMapLoadInsn::readMapLoadInsn;

constexpr bool BIRReader::isLittleEndian() {
    unsigned int val = 1;
    char *c = (char *)&val;
    return (int)*c != 0;
}

// Read 1 byte from the stream
uint8_t BIRReader::readU1() {
    uint8_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

// Read 1 byte from the stream but do not move pointer
uint8_t BIRReader::peekReadU1() {
    uint8_t value;
    value = is.peek();
    return value;
}

// Read 2 bytes from the stream
int16_t BIRReader::readS2be() {
    int16_t value;
    int16_t result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    char *p = (char *)&value;
    char tmp;
    if (isLittleEndian()) {
        tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }
    result = value;
    return result;
}

// Read 4 bytes from the stream
int32_t BIRReader::readS4be() {
    int32_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    if (isLittleEndian()) {
        uint32_t result = 0;
        result |= (value & 0x000000FF) << 24;
        result |= (value & 0x0000FF00) << 8;
        result |= (value & 0x00FF0000) >> 8;
        result |= (value & 0xFF000000) >> 24;
        value = result;
    }
    return value;
}

// Read 8 bytes from the stream
int64_t BIRReader::readS8be() {
    int64_t value;
    int64_t result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    if (isLittleEndian()) {
        char *p = (char *)&value;
        char tmp;

        tmp = p[0];
        p[0] = p[7];
        p[7] = tmp;

        tmp = p[1];
        p[1] = p[6];
        p[6] = tmp;

        tmp = p[2];
        p[2] = p[5];
        p[5] = tmp;

        tmp = p[3];
        p[3] = p[4];
        p[4] = tmp;
    }
    result = value;
    return result;
}

// Read 8 bytes from the stream for float value
double BIRReader::readS8bef() {
    double value;
    double result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    if (isLittleEndian()) {
        char *p = (char *)&value;
        char tmp;

        tmp = p[0];
        p[0] = p[7];
        p[7] = tmp;

        tmp = p[1];
        p[1] = p[6];
        p[6] = tmp;

        tmp = p[2];
        p[2] = p[5];
        p[5] = tmp;

        tmp = p[3];
        p[3] = p[4];
        p[4] = tmp;
    }
    result = value;
    return result;
}

// Search string from the constant pool based on index
std::string ConstantPoolSet::getStringCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING);
    StringCpInfo *stringCp = static_cast<StringCpInfo *>(poolEntry);
    return stringCp->getValue();
}

// Search string from the constant pool based on index
int64_t ConstantPoolSet::getIntCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER);
    IntCpInfo *intCp = static_cast<IntCpInfo *>(poolEntry);
    return intCp->getValue();
}

// Search float from the constant pool based on index
float ConstantPoolSet::getFloatCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT);
    FloatCpInfo *floatCp = static_cast<FloatCpInfo *>(poolEntry);
    return floatCp->getValue();
}

// Search boolean from the constant pool based on index
bool ConstantPoolSet::getBooleanCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN);
    BooleanCpInfo *booleanCp = static_cast<BooleanCpInfo *>(poolEntry);
    return booleanCp->getValue();
}

// Search type from the constant pool based on index
Type ConstantPoolSet::getTypeCp(int32_t index, bool voidToInt) {
    ConstantPoolEntry *poolEntry = getEntry(index);

    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);

    std::string name = getStringCp(shapeCp->getNameIndex());
    // if name is empty, create a random name anon-<5-digits>
    if (name == "")
        name.append("anon-" + std::to_string(std::rand() % 100000));

    TypeTag type = TypeTag(shapeCp->getTypeTag());

    // Handle voidToInt flag
    if (type == TYPE_TAG_NIL && voidToInt)
        return Type(TYPE_TAG_INT, name);

    // Handle Map type
    if (type == TYPE_TAG_MAP) {
        ConstantPoolEntry *shapeEntry = getEntry(shapeCp->getConstraintTypeCpIndex());
        assert(shapeEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
        ShapeCpInfo *typeShapeCp = static_cast<ShapeCpInfo *>(shapeEntry);
        return Type(type, name, Type::MapType{typeShapeCp->getTypeTag()});
    }

    // Handle Array type
    if (type == TYPE_TAG_ARRAY) {
        ConstantPoolEntry *shapeEntry = getEntry(shapeCp->getElementTypeCpIndex());
        assert(shapeEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
        ShapeCpInfo *memberShapeCp = static_cast<ShapeCpInfo *>(shapeEntry);
        return Type(type, name,
                    Type::ArrayType{memberShapeCp->getTypeTag(), (int)shapeCp->getSize(), shapeCp->getState()});
    }
    // Default return
    return Type(type, name);
}

// Get the Type tag from the constant pool based on the index passed
TypeTag ConstantPoolSet::getTypeTag(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
    return shapeCp->getTypeTag();
}

// Search type from the constant pool based on index
InvocableType ConstantPoolSet::getInvocableType(int32_t index) {

    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
    std::vector<Type> paramTypes;
    for (auto i = 0; i < shapeCp->getParamCount(); i++) {
        paramTypes.push_back(getTypeCp(shapeCp->getParam(i), false));
    }
    auto returnTypeDecl = getTypeCp(shapeCp->getReturnTypeIndex(), false);
    if (shapeCp->getRestType()) {
        auto restTypeDecl = getTypeCp(shapeCp->getRestTypeIndex(), false);
        return InvocableType(std::move(paramTypes), restTypeDecl, returnTypeDecl);
    }
    return InvocableType(std::move(paramTypes), returnTypeDecl);
}

// Read Global Variable and push it to BIRPackage
Variable BIRReader::readGlobalVar() {
    uint8_t kind = readU1();

    int32_t varDclNameCpIndex = readS4be();

    int32_t flags __attribute__((unused)) = readS4be();
    int32_t length __attribute__((unused)) = readS4be();
    uint8_t hasDoc = readU1();
    if (hasDoc) {
        int32_t descriptionCpIndex __attribute__((unused)) = readS4be();
        int32_t returnValueDescriptionCpIndex __attribute__((unused)) = readS4be();
        int32_t parameterCount __attribute__((unused)) = readS4be();
    }
    int32_t typeCpIndex = readS4be();
    auto type = constantPool->getTypeCp(typeCpIndex, false);
    return Variable(std::move(type), (constantPool->getStringCp(varDclNameCpIndex)), (VarKind)kind);
}

Variable BIRReader::readLocalVar() {
    uint8_t kind = readU1();
    int32_t typeCpIndex = readS4be();
    auto type = constantPool->getTypeCp(typeCpIndex, false);
    int32_t nameCpIndex = readS4be();

    if (kind == ARG_VAR_KIND) {
        int32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
    } else if (kind == LOCAL_VAR_KIND) {
        int32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
        int32_t endBbIdCpIndex __attribute__((unused)) = readS4be();
        int32_t startBbIdCpIndex __attribute__((unused)) = readS4be();
        int32_t instructionOffset __attribute__((unused)) = readS4be();
    }
    return Variable(std::move(type), constantPool->getStringCp(nameCpIndex), (VarKind)kind);
}

// Read Local Variable and return Variable pointer
Operand BIRReader::readOperand() {
    uint8_t ignoredVar __attribute__((unused)) = readU1();

    uint8_t kind = readU1();

    uint8_t scope __attribute__((unused)) = readU1();

    int32_t varDclNameCpIndex = readS4be();

    if ((VarKind)kind == GLOBAL_VAR_KIND) {
        int32_t packageIndex __attribute__((unused)) = readS4be();
        int32_t typeCpIndex = readS4be();
        [[maybe_unused]] Type typeDecl = constantPool->getTypeCp(typeCpIndex, false);
    }

    return Operand(constantPool->getStringCp(varDclNameCpIndex), (VarKind)kind);
}

// Read Mapping Constructor Key Value body
MapConstruct BIRReader::readMapConstructor() {

    auto kind = readU1();
    if ((MapConstrctBodyKind)kind == Spread_Field_Kind) {
        auto expr = readOperand();
        return MapConstruct(MapConstruct::SpreadField(expr));
    }
    // For Key_Value_Kind
    auto key = readOperand();
    auto value = readOperand();
    return MapConstruct(MapConstruct::KeyValue(key, value));
}

// Read TYPEDESC Insn
std::unique_ptr<TypeDescInsn> ReadTypeDescInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto lhsOp = readerRef.readOperand();
    int32_t typeCpIndex __attribute__((unused)) = readerRef.readS4be();
    return std::make_unique<TypeDescInsn>(std::move(lhsOp), currentBB);
}

// Read STRUCTURE Insn
std::unique_ptr<StructureInsn> ReadStructureInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto rhsOp = readerRef.readOperand();
    [[maybe_unused]] auto lhsOp = readerRef.readOperand();

    auto initValuesCount = readerRef.readS4be();

    if (initValuesCount == 0) {
        return std::make_unique<StructureInsn>(std::move(lhsOp), currentBB);
    }

    std::vector<MapConstruct> initValues;
    initValues.reserve(initValuesCount);
    for (auto i = 0; i < initValuesCount; i++) {
        initValues.push_back(readerRef.readMapConstructor());
    }
    return std::make_unique<StructureInsn>(std::move(lhsOp), currentBB, std::move(initValues));
}

// Read CONST_LOAD Insn
std::unique_ptr<ConstantLoadInsn> ReadConstLoadInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    int32_t typeCpIndex __attribute__((unused)) = readerRef.readS4be();
    auto lhsOp = readerRef.readOperand();

    switch (readerRef.constantPool->getTypeTag(typeCpIndex)) {
    case TYPE_TAG_INT:
    case TYPE_TAG_UNSIGNED8_INT:
    case TYPE_TAG_UNSIGNED16_INT:
    case TYPE_TAG_UNSIGNED32_INT:
    case TYPE_TAG_SIGNED8_INT:
    case TYPE_TAG_SIGNED16_INT:
    case TYPE_TAG_SIGNED32_INT:
    case TYPE_TAG_DECIMAL:
    case TYPE_TAG_BYTE: {
        int32_t valueCpIndex = readerRef.readS4be();
        return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB,
                                                  (int64_t)readerRef.constantPool->getIntCp(valueCpIndex));
    }
    case TYPE_TAG_BOOLEAN: {
        uint8_t boolean_constant = readerRef.readU1();
        if (boolean_constant == 0) {
            return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB, false);
        } else {
            return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB, true);
        }
    }
    case TYPE_TAG_FLOAT: {
        int32_t valueCpIndex = readerRef.readS4be();
        return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB,
                                                  readerRef.constantPool->getFloatCp(valueCpIndex));
    }
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING: {
        int32_t valueCpIndex = readerRef.readS4be();
        return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB,
                                                  readerRef.constantPool->getStringCp(valueCpIndex));
    }
    case TYPE_TAG_NIL:
        return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB);
    default:
        // add an error msg
        abort();
    }
}

// Read Unary Operand
std::unique_ptr<UnaryOpInsn> ReadUnaryInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto rhsOp = readerRef.readOperand();
    auto lhsOp = readerRef.readOperand();
    return std::make_unique<UnaryOpInsn>(std::move(lhsOp), currentBB, rhsOp);
}

// Read Binary Operand
std::unique_ptr<BinaryOpInsn> ReadBinaryInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto rhsOp1 = readerRef.readOperand();
    auto rhsOp2 = readerRef.readOperand();
    auto lhsOp = readerRef.readOperand();
    return std::make_unique<BinaryOpInsn>(std::move(lhsOp), currentBB, rhsOp1, rhsOp2);
}

// Read BRANCH Insn
std::unique_ptr<ConditionBrInsn> ReadCondBrInsn::readTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto lhsOp = readerRef.readOperand();
    int32_t trueBbIdNameCpIndex = readerRef.readS4be();
    int32_t falseBbIdNameCpIndex = readerRef.readS4be();

    auto trueDummybasicBlock = std::make_shared<BasicBlock>(readerRef.constantPool->getStringCp(trueBbIdNameCpIndex),
                                                            currentBB->getFunctionSharedObj());

    auto falseDummybasicBlock = std::make_shared<BasicBlock>(readerRef.constantPool->getStringCp(falseBbIdNameCpIndex),
                                                             currentBB->getFunctionSharedObj());

    return std::make_unique<ConditionBrInsn>(std::move(lhsOp), currentBB, trueDummybasicBlock, falseDummybasicBlock);
}

// Read MOV Insn
std::unique_ptr<MoveInsn> ReadMoveInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto rhsOp = readerRef.readOperand();
    auto lhsOp = readerRef.readOperand();

    return std::make_unique<MoveInsn>(std::move(lhsOp), currentBB, rhsOp);
}

// Read Function Call
std::unique_ptr<FunctionCallInsn> ReadFuncCallInsn::readTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    [[maybe_unused]] uint8_t isVirtual = readerRef.readU1();
    int32_t packageIndex __attribute__((unused)) = readerRef.readS4be();
    int32_t callNameCpIndex = readerRef.readS4be();
    string funcName = readerRef.constantPool->getStringCp(callNameCpIndex);
    int32_t argumentsCount = readerRef.readS4be();

    std::vector<Operand> fnArgs;
    for (auto i = 0; i < argumentsCount; i++) {
        fnArgs.push_back(readerRef.readOperand());
    }

    uint8_t hasLhsOperand = readerRef.readU1();
    Operand lhsOp = (hasLhsOperand > 0) ? readerRef.readOperand() : Operand("", NOT_A_KIND);

    int32_t thenBbIdNameCpIndex = readerRef.readS4be();
    auto dummybasicBlock = std::make_shared<BasicBlock>(readerRef.constantPool->getStringCp(thenBbIdNameCpIndex),
                                                        currentBB->getFunctionSharedObj());

    return std::make_unique<FunctionCallInsn>(funcName, argumentsCount, dummybasicBlock, lhsOp, std::move(fnArgs),
                                              currentBB);
}

// Read TypeCast Insn
std::unique_ptr<TypeCastInsn> ReadTypeCastInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto lhsOp = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();

    int32_t typeCpIndex = readerRef.readS4be();
    Type typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    [[maybe_unused]] uint8_t isCheckTypes = readerRef.readU1();

    return std::make_unique<TypeCastInsn>(std::move(lhsOp), currentBB, rhsOperand);
}

// Read Type Test Insn
std::unique_ptr<TypeTestInsn> ReadTypeTestInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    int32_t typeCpIndex = readerRef.readS4be();
    Type typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    auto lhsOp = readerRef.readOperand();
    [[maybe_unused]] auto rhsOperand = readerRef.readOperand();
    return std::make_unique<TypeTestInsn>(lhsOp, currentBB);
}

// Read Array Insn
std::unique_ptr<ArrayInsn> ReadArrayInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    int32_t typeCpIndex = readerRef.readS4be();
    Type typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    auto lhsOp = readerRef.readOperand();
    auto sizeOperand = readerRef.readOperand();

    // TODO handle Array init values
    auto init_values_count = readerRef.readS4be();
    for (auto i = 0; i < init_values_count; i++) {
        [[maybe_unused]] auto init_value = readerRef.readOperand();
    }
    return std::make_unique<ArrayInsn>(lhsOp, currentBB, sizeOperand);
}

// Read Array Store Insn
std::unique_ptr<ArrayStoreInsn> ReadArrayStoreInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<ArrayStoreInsn>(lhsOp, currentBB, keyOperand, rhsOperand);
}

// Read Array Load Insn
std::unique_ptr<ArrayLoadInsn> ReadArrayLoadInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    [[maybe_unused]] uint8_t optionalFieldAccess = readerRef.readU1();
    [[maybe_unused]] uint8_t fillingRead = readerRef.readU1();
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<ArrayLoadInsn>(lhsOp, currentBB, keyOperand, rhsOperand);
}

// Read Map Store Insn
std::unique_ptr<MapStoreInsn> ReadMapStoreInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<MapStoreInsn>(lhsOp, currentBB, keyOperand, rhsOperand);
}

// Read Map Load Insn
std::unique_ptr<MapLoadInsn> ReadMapLoadInsn::readNonTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    [[maybe_unused]] uint8_t optionalFieldAccess = readerRef.readU1();
    [[maybe_unused]] uint8_t fillingRead = readerRef.readU1();
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<MapLoadInsn>(lhsOp, currentBB, keyOperand, rhsOperand);
}

std::unique_ptr<GoToInsn> ReadGoToInsn::readTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    int32_t nameId = readerRef.readS4be();
    auto dummybasicBlock =
        std::make_shared<BasicBlock>(readerRef.constantPool->getStringCp(nameId), currentBB->getFunctionSharedObj());
    return std::make_unique<GoToInsn>(dummybasicBlock, currentBB);
}

std::unique_ptr<ReturnInsn> ReadReturnInsn::readTerminatorInsn(std::shared_ptr<BasicBlock> currentBB) {
    return std::make_unique<ReturnInsn>(currentBB);
}

// Read an Instruction - either a NonTerminatorInsn or TerminatorInsn from the BIR
void BIRReader::readInsn(std::shared_ptr<BasicBlock> basicBlock) {
    int32_t sLine = readS4be();
    int32_t eLine = readS4be();
    int32_t sCol = readS4be();
    int32_t eCol = readS4be();
    int32_t sourceFileCpIndex = readS4be();
    Location location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    InstructionKind insnKind = (InstructionKind)readU1();

    switch (insnKind) {
    case INSTRUCTION_KIND_NEW_TYPEDESC: {
        [[maybe_unused]] auto typeDescInsn = ReadTypeDescInsn::readTypeDescInsn.readNonTerminatorInsn(basicBlock);
        break;
    }
    case INSTRUCTION_KIND_NEW_STRUCTURE: {
        auto structureInsn = ReadStructureInsn::readStructureInsn.readNonTerminatorInsn(basicBlock);
        structureInsn->setLocation(location);
        basicBlock->addNonTermInsn(std::move(structureInsn));
        break;
    }
    case INSTRUCTION_KIND_CONST_LOAD: {
        basicBlock->addNonTermInsn(ReadConstLoadInsn::readConstLoadInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_GOTO: {
        basicBlock->setTerminatorInsn(ReadGoToInsn::readGoToInsn.readTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_RETURN: {
        basicBlock->setTerminatorInsn(ReadReturnInsn::readReturnInsn.readTerminatorInsn(basicBlock));
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
        auto binaryOpInsn = ReadBinaryInsn::readBinaryInsn.readNonTerminatorInsn(basicBlock);
        binaryOpInsn->setInstKind(insnKind);
        basicBlock->addNonTermInsn(std::move(binaryOpInsn));
        break;
    }
    case INSTRUCTION_KIND_UNARY_NEG:
    case INSTRUCTION_KIND_UNARY_NOT: {
        basicBlock->addNonTermInsn(ReadUnaryInsn::readUnaryInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
        basicBlock->setTerminatorInsn(ReadCondBrInsn::readCondBrInsn.readTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_MOVE: {
        basicBlock->addNonTermInsn(ReadMoveInsn::readMoveInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_CALL: {
        basicBlock->setTerminatorInsn(ReadFuncCallInsn::readFuncCallInsn.readTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_TYPE_CAST: {
        basicBlock->addNonTermInsn(ReadTypeCastInsn::readTypeCastInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_TYPE_TEST: {
        basicBlock->addNonTermInsn(ReadTypeTestInsn::readTypeTestInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_NEW_ARRAY: {
        basicBlock->addNonTermInsn(ReadArrayInsn::readArrayInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_ARRAY_STORE: {
        basicBlock->addNonTermInsn(ReadArrayStoreInsn::readArrayStoreInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_ARRAY_LOAD: {
        basicBlock->addNonTermInsn(ReadArrayLoadInsn::readArrayLoadInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_MAP_STORE: {
        basicBlock->addNonTermInsn(ReadMapStoreInsn::readMapStoreInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_MAP_LOAD: {
        basicBlock->addNonTermInsn(ReadMapLoadInsn::readMapLoadInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    default:
        fprintf(stderr, "%s:%d Invalid Insn Kind for Reader.\n", __FILE__, __LINE__);
        break;
    }
}

// Read Basic Block from the BIR
std::shared_ptr<BasicBlock> BIRReader::readBasicBlock(std::shared_ptr<Function> birFunction) {
    int32_t nameCpIndex = readS4be();
    auto basicBlock = std::make_shared<BasicBlock>(constantPool->getStringCp(nameCpIndex), birFunction);

    int32_t insnCount = readS4be();
    for (auto i = 0; i < insnCount; i++) {
        // Read an Instruction and adds it to basicBlock
        readInsn(basicBlock);
    }
    return basicBlock;
}

bool BIRReader::ignoreFunction(std::string funcName) {
    std::array<std::string, 3> ignoreNames{"..<init>", "..<start>", "..<stop>"};
    bool ignoreFunction = false;
    for (const auto &name : ignoreNames) {
        if (funcName.rfind(name, 0) == 0) {
            ignoreFunction = true;
            break;
        }
    }
    return ignoreFunction;
}

// Reads BIR function
std::shared_ptr<Function> BIRReader::readFunction(std::shared_ptr<Package> package) {

    // Read debug info
    int32_t sLine = readS4be();
    int32_t eLine = readS4be();
    int32_t sCol = readS4be();
    int32_t eCol = readS4be();
    int32_t sourceFileCpIndex = readS4be();
    Location location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    // TODO should not set src for every function
    package->setSrcFileName(constantPool->getStringCp(sourceFileCpIndex));

    int32_t nameCpIndex = readS4be();
    std::string functionName = constantPool->getStringCp(nameCpIndex);
    int32_t workdernameCpIndex = readS4be();
    int32_t flags = readS4be();
    int32_t typeCpIndex = readS4be();
    [[maybe_unused]] auto invocable_type = constantPool->getInvocableType(typeCpIndex);
    auto birFunction =
        std::make_shared<Function>(package, functionName, constantPool->getStringCp(workdernameCpIndex), flags);
    birFunction->setLocation(location);

    int64_t annotationLength __attribute__((unused)) = readS8be();
    int32_t annotationAttachments __attribute__((unused)) = readS4be();
    int32_t requiredParamCount = readS4be();

    // Set function param here and then fill remaining values from the default Params
    std::vector<nballerina::Operand> functionParams;
    functionParams.reserve(requiredParamCount);
    for (auto i = 0; i < requiredParamCount; i++) {
        int32_t paramNameCpIndex = readS4be();
        functionParams.push_back(Operand(constantPool->getStringCp(paramNameCpIndex), ARG_VAR_KIND));
        int32_t paramFlags __attribute__((unused)) = readS4be();
    }

    uint8_t hasRestParam __attribute__((unused)) = readU1();
    // if (!hasRestParam)
    //   birFunction->setRestParam(NULL);

    uint8_t hasReceiver __attribute__((unused)) = readU1();
    // if (!hasReceiver)
    //   birFunction->setReceiver(NULL);

    int64_t taintTableLength = readS8be();
    std::vector<char> taint(taintTableLength);
    is.read(&taint[0], taintTableLength);

    int32_t docLength = readS4be();
    std::vector<char> doc(docLength);
    is.read(&doc[0], docLength);

    int64_t functionBodyLength __attribute__((unused)) = readS8be();
    int32_t argsCount __attribute__((unused)) = readS4be();
    uint8_t hasReturnVar = readU1();

    if (hasReturnVar) {
        uint8_t kind = readU1();
        int32_t typeCpIndex = readS4be();
        auto type = constantPool->getTypeCp(typeCpIndex, false);
        int32_t nameCpIndex = readS4be();
        birFunction->setReturnVar(Variable(std::move(type), constantPool->getStringCp(nameCpIndex), (VarKind)kind));
    }

    int32_t defaultParamValue = readS4be();
    for (auto i = 0; i < defaultParamValue; i++) {
        uint8_t kind = readU1();
        int32_t typeCpIndex = readS4be();
        birFunction->insertParam(FunctionParam(functionParams[i], constantPool->getTypeCp(typeCpIndex, false)));

        int32_t nameCpIndex __attribute__((unused)) = readS4be();
        if (kind == ARG_VAR_KIND) {
            int32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
        }
        uint8_t hasDefaultExpr __attribute__((unused)) = readU1();
    }

    int32_t localVarCount = readS4be();
    for (auto i = 0; i < localVarCount; i++) {
        birFunction->insertLocalVar(readLocalVar());
    }
    // Ignore default param values
    for (auto i = 0; i < defaultParamValue; i++) {
        int32_t basicBlocksCount __attribute__((unused)) = readS4be();
    }

    int32_t BBCount = readS4be();
    std::shared_ptr<BasicBlock> previousBB;
    for (auto i = 0; i < BBCount; i++) {
        auto basicBlock = readBasicBlock(birFunction);
        birFunction->insertBasicBlock(basicBlock);
        // Create links between the basic blocks
        if (previousBB) {
            previousBB->setNextBB(basicBlock);
        }
        previousBB = basicBlock;
    }

    // Patching the Instructions
    birFunction->patchBasicBlocks();

    int32_t errorEntriesCount __attribute__((unused)) = readS4be();
    int32_t channelsLength __attribute__((unused)) = readS4be();
    return birFunction;
}

StringCpInfo::StringCpInfo() { setTag(TAG_ENUM_CP_ENTRY_STRING); }

void StringCpInfo::read() {
    int32_t stringLength = readerRef.readS4be();
    std::vector<char> result(stringLength);
    readerRef.is.read(&result[0], stringLength);
    value = std::string(result.begin(), result.end());
}

ShapeCpInfo::ShapeCpInfo() { setTag(TAG_ENUM_CP_ENTRY_SHAPE); }

void ShapeCpInfo::read() {
    shapeLength = readerRef.readS4be();
    typeTag = TypeTag(readerRef.readU1());
    nameIndex = readerRef.readS4be();
    typeFlag = readerRef.readS4be();
    typeSpecialFlag = readerRef.readS4be();

    int32_t shapeLengthTypeInfo = shapeLength - 13;

    switch (typeTag) {
    case TYPE_TAG_INVOKABLE: {
        paramCount = readerRef.readS4be();
        for (auto i = 0; i < paramCount; i++) {
            int32_t paramTypeCpIndex = readerRef.readS4be();
            addParam(paramTypeCpIndex);
        }
        hasRestType = readerRef.readU1();
        if (hasRestType) {
            restTypeIndex = readerRef.readS4be();
        }
        returnTypeIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_MAP: {
        assert(shapeLengthTypeInfo == 4);
        constraintTypeCpIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_INT:
    case TYPE_TAG_BYTE:
    case TYPE_TAG_FLOAT:
    case TYPE_TAG_DECIMAL:
    case TYPE_TAG_STRING:
    case TYPE_TAG_BOOLEAN:
    case TYPE_TAG_JSON:
    case TYPE_TAG_XML:
    case TYPE_TAG_TABLE:
    case TYPE_TAG_NIL:
    case TYPE_TAG_ANYDATA:
    case TYPE_TAG_RECORD:
    case TYPE_TAG_TYPEDESC:
    case TYPE_TAG_STREAM:
    case TYPE_TAG_ANY:
    case TYPE_TAG_ENDPOINT:
    case TYPE_TAG_UNION:
    case TYPE_TAG_INTERSECTION:
    case TYPE_TAG_PACKAGE:
    case TYPE_TAG_NONE:
    case TYPE_TAG_VOID:
    case TYPE_TAG_XMLNS:
    case TYPE_TAG_ANNOTATION:
    case TYPE_TAG_SEMANTIC_ERROR:
    case TYPE_TAG_ERROR:
    case TYPE_TAG_ITERATOR:
    case TYPE_TAG_TUPLE:
    case TYPE_TAG_FUTURE:
    case TYPE_TAG_FINITE:
    case TYPE_TAG_OBJECT:
    case TYPE_TAG_SERVICE:
    case TYPE_TAG_BYTE_ARRAY:
    case TYPE_TAG_FUNCTION_POINTER:
    case TYPE_TAG_HANDLE:
    case TYPE_TAG_READONLY:
    case TYPE_TAG_SIGNED32_INT:
    case TYPE_TAG_SIGNED16_INT:
    case TYPE_TAG_SIGNED8_INT:
    case TYPE_TAG_UNSIGNED32_INT:
    case TYPE_TAG_UNSIGNED16_INT:
    case TYPE_TAG_UNSIGNED8_INT:
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_XML_ELEMENT:
    case TYPE_TAG_XML_PI:
    case TYPE_TAG_XML_COMMENT:
    case TYPE_TAG_XML_TEXT:
    case TYPE_TAG_NEVER:
    case TYPE_TAG_NULL_SET:
    case TYPE_TAG_PARAMETERIZED_TYPE: {
        std::vector<char> result(shapeLengthTypeInfo);
        readerRef.is.read(&result[0], shapeLengthTypeInfo);
        break;
    }
    case TYPE_TAG_ARRAY: {
        state = readerRef.readU1();
        size = readerRef.readS4be();
        elementTypeCpIndex = readerRef.readS4be();
        break;
    }
    default:
        fprintf(stderr, "%s:%d Invalid Type Tag in shape.\n", __FILE__, __LINE__);
        fprintf(stderr, "%d is the Type Tag.\n", typeTag);
        break;
    }
}

PackageCpInfo::PackageCpInfo() {
    orgIndex = 0;
    nameIndex = 0;
    versionIndex = 0;
    setTag(TAG_ENUM_CP_ENTRY_PACKAGE);
}

void PackageCpInfo::read() {
    orgIndex = readerRef.readS4be();
    nameIndex = readerRef.readS4be();
    versionIndex = readerRef.readS4be();
}

IntCpInfo::IntCpInfo() { setTag(TAG_ENUM_CP_ENTRY_INTEGER); }

void IntCpInfo::read() { value = readerRef.readS8be(); }

BooleanCpInfo::BooleanCpInfo() { setTag(TAG_ENUM_CP_ENTRY_BOOLEAN); }

void BooleanCpInfo::read() { value = readerRef.readU1(); }

FloatCpInfo::FloatCpInfo() { setTag(TAG_ENUM_CP_ENTRY_FLOAT); }

void FloatCpInfo::read() { value = readerRef.readS8bef(); }

ByteCpInfo::ByteCpInfo() { setTag(TAG_ENUM_CP_ENTRY_BYTE); }

void ByteCpInfo::read() { value = readerRef.readU1(); }

void ConstantPoolSet::read() {
    int constantPoolEntries = readerRef.readS4be();
    poolEntries = std::vector<std::unique_ptr<ConstantPoolEntry>>();
    poolEntries.reserve(constantPoolEntries);
    for (auto i = 0; i < constantPoolEntries; i++) {
        ConstantPoolEntry::tagEnum tag = static_cast<ConstantPoolEntry::tagEnum>(readerRef.readU1());
        switch (tag) {
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
            auto poolEntry = std::make_unique<PackageCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE: {
            auto poolEntry = std::make_unique<ShapeCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING: {
            auto poolEntry = std::make_unique<StringCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER: {
            auto poolEntry = std::make_unique<IntCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT: {
            auto poolEntry = std::make_unique<FloatCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN: {
            auto poolEntry = std::make_unique<BooleanCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BYTE: {
            auto poolEntry = std::make_unique<ByteCpInfo>();
            poolEntry->read();
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        default:
            break;
        }
    }
}

// Assigns Type Decl to function parameters
/*
void BIRReader::patchTypesToFuncParam() {
  for (size_t i = 0; i < birPackage.getNumFunctions(); i++) {
    Function *curFunc = birPackage.getFunction(i);
    for (size_t i = 0; i < curFunc->numBasicBlocks(); i++) {
      BasicBlock *birBasicBlock = curFunc->getBasicBlock(i);
      for (size_t i = 0; i < birBasicBlock->numInsns(); i++) {
        TerminatorInsn *terminator = birBasicBlock->getTerminatorInsn();
        if (terminator && terminator->isPatched()) {
          switch (terminator->getInstKind()) {
          case INSTRUCTION_KIND_CALL: {
            FunctionCallInsn *Terminator =
                (static_cast<FunctionCallInsn *>(terminator));
            for (int i = 0; i < Terminator->getArgCount(); i++) {
              Function *patchCallFunction =
                  birPackage.getFunction(Terminator->getFunctionName());
              InvocableType *invokableType =
                  patchCallFunction->getInvokableType();
              for (size_t i = 0; i < invokableType->getParamTypeCount(); i++) {
                Type *typeDecl = invokableType->getParamType(i);
                Operand *param = Terminator->getArgumentFromList(i);
                // param->setType(typeDecl);
              }
            }
            break;
          }
          default:
            fprintf(stderr,
                    "%s:%d Invalid Insn Kind for assigning Type to "
                    "FuncCall Insn.\n",
                    __FILE__, __LINE__);
            break;
          }
        }
      }
    }
  }
}
*/

std::shared_ptr<nballerina::Package> BIRReader::readModule() {
    int32_t idCpIndex = readS4be();
    ConstantPoolEntry *poolEntry = constantPool->getEntry(idCpIndex);
    auto birPackage = std::make_shared<Package>();

    switch (poolEntry->getTag()) {
    case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
        PackageCpInfo *packageEntry = static_cast<PackageCpInfo *>(poolEntry);
        poolEntry = constantPool->getEntry(packageEntry->getOrgIndex());
        StringCpInfo *stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->setOrgName(stringCp->getValue());

        poolEntry = constantPool->getEntry(packageEntry->getNameIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->setPackageName(stringCp->getValue());

        poolEntry = constantPool->getEntry(packageEntry->getVersionIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->setVersion(stringCp->getValue());
        break;
    }
    default:
        break;
    }

    // The following three are read into unused variables so that the file
    // pointer advances to the data that we need next.
    int32_t importCount __attribute__((unused));
    int32_t constCount __attribute__((unused));
    int32_t typeDefinitionCount __attribute__((unused));

    importCount = readS4be();
    constCount = readS4be();
    typeDefinitionCount = readS4be();

    int32_t globalVarCount = readS4be();
    if (globalVarCount > 0) {
        for (auto i = 0; i < globalVarCount; i++) {
            birPackage->insertGlobalVar(readGlobalVar());
        }
    }

    int32_t typeDefinitionBodiesCount __attribute__((unused)) = readS4be();
    int32_t functionCount = readS4be();

    // Push all the functions in BIRpackage except __init, __start & __stop
    for (auto i = 0; i < functionCount; i++) {
        auto curFunc = readFunction(birPackage);
        if (!ignoreFunction(curFunc->getName())) {
            birPackage->insertFunction(curFunc);
        }
    }

    int32_t annotationsSize __attribute__((unused)) = readS4be();

    // Assign typedecl to function param of call Insn
    // patchTypesToFuncParam();

    return birPackage;
}

std::shared_ptr<nballerina::Package> BIRReader::deserialize() {
    // Read Constant Pool
    ConstantPoolSet *constantPoolSet = new ConstantPoolSet();
    constantPoolSet->read();
    setConstantPool(constantPoolSet);

    // Read Module
    return readModule();
}
