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
#include <queue>

namespace nballerina {

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

bool BIRReader::isLittleEndian() {
    unsigned int val = 1;
    auto *c = reinterpret_cast<char *>(&val);
    return (int)*c != 0;
}

template <typename T>
T swapEndian(T u) {
    static_assert(CHAR_BIT == 8);
    union {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++) {
        dest.u8[k] = source.u8[sizeof(T) - k - 1];
    }
    return dest.u;
}

// Read 1 byte from the stream
uint8_t BIRReader::readU1() {
    uint8_t value = 0;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

// Read 1 byte from the stream but do not move pointer
uint8_t BIRReader::peekReadU1() {
    uint8_t value = 0;
    value = is.peek();
    return value;
}

// Read 2 bytes from the stream
int16_t BIRReader::readS2be() {
    int16_t value = 0;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<int16_t>(value);
}

// Read 4 bytes from the stream
int32_t BIRReader::readS4be() {
    int32_t value = 0;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<int32_t>(value);
}

// Read 8 bytes from the stream
int64_t BIRReader::readS8be() {
    int64_t value = 0;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<int64_t>(value);
}

// Read 8 bytes from the stream for float value
double BIRReader::readS8bef() {
    double value = NAN;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<double>(value);
}

// Search string from the constant pool based on index
std::string ConstantPoolSet::getStringCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING);
    auto *stringCp = static_cast<StringCpInfo *>(poolEntry);
    return stringCp->getValue();
}

// Search string from the constant pool based on index
int64_t ConstantPoolSet::getIntCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER);
    auto *intCp = static_cast<IntCpInfo *>(poolEntry);
    return intCp->getValue();
}

// Search float from the constant pool based on index
double ConstantPoolSet::getFloatCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT);
    auto *floatCp = static_cast<FloatCpInfo *>(poolEntry);
    return floatCp->getValue();
}

// Search boolean from the constant pool based on index
bool ConstantPoolSet::getBooleanCp(int32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN);
    auto *booleanCp = static_cast<BooleanCpInfo *>(poolEntry);
    return (booleanCp->getValue() != 0u);
}

// Search type from the constant pool based on index
Type ConstantPoolSet::getTypeCp(int32_t index, bool voidToInt) {
    ConstantPoolEntry *poolEntry = getEntry(index);

    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    auto *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);

    std::string name = getStringCp(shapeCp->getNameIndex());
    // if name is empty, create a random name anon-<5-digits>
    if (name.empty()) {
        name.append("anon-" + std::to_string(std::rand() % 100000));
    }
    auto type = TypeTag(shapeCp->getTypeTag());

    // Handle voidToInt flag
    if (type == TYPE_TAG_NIL && voidToInt) {
        return Type(TYPE_TAG_INT, name);
    }
    // Handle Map type
    if (type == TYPE_TAG_MAP) {
        ConstantPoolEntry *shapeEntry = getEntry(shapeCp->getConstraintTypeCpIndex());
        assert(shapeEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
        auto *typeShapeCp = static_cast<ShapeCpInfo *>(shapeEntry);
        return Type(type, name, Type::MapType{typeShapeCp->getTypeTag()});
    }

    // Handle Array type
    if (type == TYPE_TAG_ARRAY) {
        ConstantPoolEntry *shapeEntry = getEntry(shapeCp->getElementTypeCpIndex());
        assert(shapeEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
        auto *memberShapeCp = static_cast<ShapeCpInfo *>(shapeEntry);
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
    auto *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
    return shapeCp->getTypeTag();
}

// Search type from the constant pool based on index
InvocableType ConstantPoolSet::getInvocableType(int32_t index) {

    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    auto *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
    auto paramCount = shapeCp->getParamCount();
    std::vector<Type> paramTypes;
    paramTypes.reserve(paramCount);
    for (auto i = 0; i < paramCount; i++) {
        paramTypes.push_back(getTypeCp(shapeCp->getParam(i), false));
    }
    auto returnTypeDecl = getTypeCp(shapeCp->getReturnTypeIndex(), false);
    if (shapeCp->getRestType() != 0U) {
        auto restTypeDecl = getTypeCp(shapeCp->getRestTypeIndex(), false);
        return InvocableType(std::move(paramTypes), restTypeDecl, returnTypeDecl);
    }
    return InvocableType(std::move(paramTypes), returnTypeDecl);
}

// Read Global Variable and push it to BIRPackage
void BIRReader::readGlobalVar(Package &birPackage) {
    uint8_t kind = readU1();

    int32_t varDclNameCpIndex = readS4be();

    [[maybe_unused]] int64_t flags = readS8be();
    [[maybe_unused]] uint8_t origin = readU1();
    // Markdown
    int32_t docLength = readS4be();
    is.ignore(docLength);

    int32_t typeCpIndex = readS4be();
    auto type = constantPool->getTypeCp(typeCpIndex, false);
    birPackage.globalVars.emplace_back(std::move(type), constantPool->getStringCp(varDclNameCpIndex), (VarKind)kind);
}

void BIRReader::readLocalVar(Function &birFunction) {
    uint8_t kind = readU1();
    int32_t typeCpIndex = readS4be();
    auto type = constantPool->getTypeCp(typeCpIndex, false);
    int32_t nameCpIndex = readS4be();

    if (kind == ARG_VAR_KIND) {
        [[maybe_unused]] int32_t metaVarNameCpIndex = readS4be();
    } else if (kind == LOCAL_VAR_KIND) {
        // enclosing basic block id
        [[maybe_unused]] int32_t metaVarNameCpIndex = readS4be();
        [[maybe_unused]] int32_t endBbIdCpIndex = readS4be();
        [[maybe_unused]] int32_t startBbIdCpIndex = readS4be();
        [[maybe_unused]] int32_t instructionOffset = readS4be();
    }
    birFunction.localVars.emplace_back(std::move(type), constantPool->getStringCp(nameCpIndex), (VarKind)kind);
}

// Read Local Variable and return Variable pointer
Operand BIRReader::readOperand() {
    [[maybe_unused]] uint8_t ignoredVar = readU1();

    uint8_t kind = readU1();

    [[maybe_unused]] uint8_t scope = readU1();

    int32_t varDclNameCpIndex = readS4be();

    if ((VarKind)kind == GLOBAL_VAR_KIND) {
        [[maybe_unused]] int32_t packageIndex = readS4be();
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
        return MapConstruct(MapConstruct::SpreadField(std::move(expr)));
    }
    // For Key_Value_Kind
    auto key = readOperand();
    auto value = readOperand();
    return MapConstruct(MapConstruct::KeyValue(std::move(key), std::move(value)));
}

// Read TYPEDESC Insn
std::unique_ptr<TypeDescInsn> ReadTypeDescInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto lhsOp = readerRef.readOperand();
    [[maybe_unused]] int32_t typeCpIndex = readerRef.readS4be();
    return std::make_unique<TypeDescInsn>(std::move(lhsOp), currentBB);
}

// Read STRUCTURE Insn
std::unique_ptr<StructureInsn> ReadStructureInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
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
std::unique_ptr<ConstantLoadInsn> ReadConstLoadInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    [[maybe_unused]] int32_t typeCpIndex = readerRef.readS4be();
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
        }
        return std::make_unique<ConstantLoadInsn>(std::move(lhsOp), currentBB, true);
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
std::unique_ptr<UnaryOpInsn> ReadUnaryInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto rhsOp = readerRef.readOperand();
    auto lhsOp = readerRef.readOperand();
    return std::make_unique<UnaryOpInsn>(std::move(lhsOp), currentBB, std::move(rhsOp));
}

// Read Binary Operand
std::unique_ptr<BinaryOpInsn> ReadBinaryInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto rhsOp1 = readerRef.readOperand();
    auto rhsOp2 = readerRef.readOperand();
    auto lhsOp = readerRef.readOperand();
    return std::make_unique<BinaryOpInsn>(std::move(lhsOp), currentBB, std::move(rhsOp1), std::move(rhsOp2));
}

// Read BRANCH Insn
std::unique_ptr<ConditionBrInsn> ReadCondBrInsn::readTerminatorInsn(BasicBlock &currentBB) {
    auto lhsOp = readerRef.readOperand();
    int32_t trueBbIdNameCpIndex = readerRef.readS4be();
    int32_t falseBbIdNameCpIndex = readerRef.readS4be();

    return std::make_unique<ConditionBrInsn>(std::move(lhsOp), currentBB,
                                             readerRef.constantPool->getStringCp(trueBbIdNameCpIndex),
                                             readerRef.constantPool->getStringCp(falseBbIdNameCpIndex));
}

// Read MOV Insn
std::unique_ptr<MoveInsn> ReadMoveInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto rhsOp = readerRef.readOperand();
    auto lhsOp = readerRef.readOperand();

    return std::make_unique<MoveInsn>(std::move(lhsOp), currentBB, std::move(rhsOp));
}

// Read Function Call
std::unique_ptr<FunctionCallInsn> ReadFuncCallInsn::readTerminatorInsn(BasicBlock &currentBB) {
    [[maybe_unused]] uint8_t isVirtual = readerRef.readU1();
    [[maybe_unused]] int32_t packageIndex = readerRef.readS4be();
    int32_t callNameCpIndex = readerRef.readS4be();
    std::string funcName = readerRef.constantPool->getStringCp(callNameCpIndex);
    int32_t argumentsCount = readerRef.readS4be();

    std::vector<Operand> fnArgs;
    fnArgs.reserve(argumentsCount);
    for (auto i = 0; i < argumentsCount; i++) {
        fnArgs.push_back(readerRef.readOperand());
    }

    uint8_t hasLhsOperand = readerRef.readU1();
    Operand lhsOp = (hasLhsOperand > 0) ? readerRef.readOperand() : Operand("", NOT_A_KIND);

    auto thenBbIdNameCpIndex = readerRef.readS4be();

    return std::make_unique<FunctionCallInsn>(currentBB, readerRef.constantPool->getStringCp(thenBbIdNameCpIndex),
                                              std::move(lhsOp), std::move(funcName), argumentsCount, std::move(fnArgs));
}

// Read TypeCast Insn
std::unique_ptr<TypeCastInsn> ReadTypeCastInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto lhsOp = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();

    int32_t typeCpIndex = readerRef.readS4be();
    Type typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    [[maybe_unused]] uint8_t isCheckTypes = readerRef.readU1();

    return std::make_unique<TypeCastInsn>(std::move(lhsOp), currentBB, std::move(rhsOperand));
}

// Read Type Test Insn
std::unique_ptr<TypeTestInsn> ReadTypeTestInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    int32_t typeCpIndex = readerRef.readS4be();
    Type typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    auto lhsOp = readerRef.readOperand();
    [[maybe_unused]] auto rhsOperand = readerRef.readOperand();
    return std::make_unique<TypeTestInsn>(std::move(lhsOp), currentBB);
}

// Read Array Insn
std::unique_ptr<ArrayInsn> ReadArrayInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    int32_t typeCpIndex = readerRef.readS4be();
    Type typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    auto lhsOp = readerRef.readOperand();
    auto sizeOperand = readerRef.readOperand();

    // TODO handle Array init values
    auto init_values_count = readerRef.readS4be();
    for (auto i = 0; i < init_values_count; i++) {
        [[maybe_unused]] auto init_value = readerRef.readOperand();
    }
    return std::make_unique<ArrayInsn>(std::move(lhsOp), currentBB, std::move(sizeOperand));
}

// Read Array Store Insn
std::unique_ptr<ArrayStoreInsn> ReadArrayStoreInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<ArrayStoreInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand));
}

// Read Array Load Insn
std::unique_ptr<ArrayLoadInsn> ReadArrayLoadInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    [[maybe_unused]] uint8_t optionalFieldAccess = readerRef.readU1();
    [[maybe_unused]] uint8_t fillingRead = readerRef.readU1();
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<ArrayLoadInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand));
}

// Read Map Store Insn
std::unique_ptr<MapStoreInsn> ReadMapStoreInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<MapStoreInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand));
}

// Read Map Load Insn
std::unique_ptr<MapLoadInsn> ReadMapLoadInsn::readNonTerminatorInsn(BasicBlock &currentBB) {
    [[maybe_unused]] uint8_t optionalFieldAccess = readerRef.readU1();
    [[maybe_unused]] uint8_t fillingRead = readerRef.readU1();
    auto lhsOp = readerRef.readOperand();
    auto keyOperand = readerRef.readOperand();
    auto rhsOperand = readerRef.readOperand();
    return std::make_unique<MapLoadInsn>(std::move(lhsOp), currentBB, std::move(keyOperand), std::move(rhsOperand));
}

std::unique_ptr<GoToInsn> ReadGoToInsn::readTerminatorInsn(BasicBlock &currentBB) {
    auto nameId = readerRef.readS4be();
    return std::make_unique<GoToInsn>(currentBB, readerRef.constantPool->getStringCp(nameId));
}

std::unique_ptr<ReturnInsn> ReadReturnInsn::readTerminatorInsn(BasicBlock &currentBB) {
    return std::make_unique<ReturnInsn>(currentBB);
}

// Read an Instruction - either a NonTerminatorInsn or TerminatorInsn from the BIR
void BIRReader::readInsn(BasicBlock &basicBlock) {
    int32_t sourceFileCpIndex = readS4be();
    int32_t sLine = readS4be();
    int32_t sCol = readS4be();
    int32_t eLine = readS4be();
    int32_t eCol = readS4be();
    Location location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    auto insnKind = (InstructionKind)readU1();

    switch (insnKind) {
    case INSTRUCTION_KIND_NEW_TYPEDESC: {
        [[maybe_unused]] auto typeDescInsn = ReadTypeDescInsn::readTypeDescInsn.readNonTerminatorInsn(basicBlock);
        break;
    }
    case INSTRUCTION_KIND_NEW_STRUCTURE: {
        auto structureInsn = ReadStructureInsn::readStructureInsn.readNonTerminatorInsn(basicBlock);
        structureInsn->setLocation(location);
        basicBlock.addNonTermInsn(std::move(structureInsn));
        break;
    }
    case INSTRUCTION_KIND_CONST_LOAD: {
        basicBlock.addNonTermInsn(ReadConstLoadInsn::readConstLoadInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_GOTO: {
        basicBlock.setTerminatorInsn(ReadGoToInsn::readGoToInsn.readTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_RETURN: {
        basicBlock.setTerminatorInsn(ReadReturnInsn::readReturnInsn.readTerminatorInsn(basicBlock));
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
        basicBlock.addNonTermInsn(std::move(binaryOpInsn));
        break;
    }
    case INSTRUCTION_KIND_UNARY_NEG:
    case INSTRUCTION_KIND_UNARY_NOT: {
        auto unaryOpInsn = ReadUnaryInsn::readUnaryInsn.readNonTerminatorInsn(basicBlock);
        unaryOpInsn->setInstKind(insnKind);
        basicBlock.addNonTermInsn(std::move(unaryOpInsn));
        break;
    }
    case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
        basicBlock.setTerminatorInsn(ReadCondBrInsn::readCondBrInsn.readTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_MOVE: {
        basicBlock.addNonTermInsn(ReadMoveInsn::readMoveInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_CALL: {
        basicBlock.setTerminatorInsn(ReadFuncCallInsn::readFuncCallInsn.readTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_TYPE_CAST: {
        basicBlock.addNonTermInsn(ReadTypeCastInsn::readTypeCastInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_TYPE_TEST: {
        basicBlock.addNonTermInsn(ReadTypeTestInsn::readTypeTestInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_NEW_ARRAY: {
        basicBlock.addNonTermInsn(ReadArrayInsn::readArrayInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_ARRAY_STORE: {
        basicBlock.addNonTermInsn(ReadArrayStoreInsn::readArrayStoreInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_ARRAY_LOAD: {
        basicBlock.addNonTermInsn(ReadArrayLoadInsn::readArrayLoadInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_MAP_STORE: {
        basicBlock.addNonTermInsn(ReadMapStoreInsn::readMapStoreInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    case INSTRUCTION_KIND_MAP_LOAD: {
        basicBlock.addNonTermInsn(ReadMapLoadInsn::readMapLoadInsn.readNonTerminatorInsn(basicBlock));
        break;
    }
    default:
        abort();
        break;
    }
}

// Read Basic Block from the BIR
void BIRReader::readBasicBlock(Function &birFunction, bool ignore) {
    int32_t nameCpIndex = readS4be();
    birFunction.basicBlocks.emplace_back(constantPool->getStringCp(nameCpIndex), &birFunction);
    auto &basicBlock = birFunction.basicBlocks.back();

    int32_t insnCount = readS4be();
    basicBlock.instructions.reserve(insnCount);
    for (auto i = 0; i < insnCount; i++) {
        readInsn(basicBlock);
    }
    if (ignore) {
        birFunction.basicBlocks.pop_back();
    }
}

bool BIRReader::ignoreFunction(const std::string &funcName) {
    std::array<std::string, 6> ignoreNames{".<init>", ".<start>", ".<stop>", "..<init>", "..<start>", "..<stop>"};
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
void BIRReader::readFunction(Package &package, bool ignore) {

    // Read debug info
    // position
    int32_t sourceFileCpIndex = readS4be();
    int32_t sLine = readS4be();
    int32_t sCol = readS4be();
    int32_t eLine = readS4be();
    int32_t eCol = readS4be();
    Location location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    // TODO should not set src for every function
    package.sourceFileName = constantPool->getStringCp(sourceFileCpIndex);

    int32_t nameCpIndex = readS4be();
    std::string functionName = constantPool->getStringCp(nameCpIndex);
    int32_t workdernameCpIndex = readS4be();
    int32_t flags = readS8be();
    [[maybe_unused]] uint8_t origin = readU1();
    int32_t typeCpIndex = readS4be();
    [[maybe_unused]] auto invocable_type = constantPool->getInvocableType(typeCpIndex);

    package.functions.emplace_back(&package, functionName, constantPool->getStringCp(workdernameCpIndex), flags);
    auto &birFunction = package.functions.back();
    birFunction.setLocation(location);

    // annotation_attachments_content
    int64_t annotationLength = readS8be(); // annotation_attachments_content_length
    is.ignore(annotationLength);

    int32_t requiredParamCount = readS4be();

    // Set function param here and then fill remaining values from the default Params
    std::queue<Operand> functionParams;
    for (auto i = 0; i < requiredParamCount; i++) {
        int32_t paramNameCpIndex = readS4be();
        functionParams.emplace(Operand(constantPool->getStringCp(paramNameCpIndex), ARG_VAR_KIND));
        [[maybe_unused]] int64_t paramFlags = readS8be();
    }

    uint8_t hasRestParam = readU1();
    [[maybe_unused]] int32_t restParamNameCpIndex = 0;
    if (hasRestParam != 0U) {
        restParamNameCpIndex = readS4be();
    }

    [[maybe_unused]] uint8_t hasReceiver = readU1();
    if (hasReceiver != 0U) {
        [[maybe_unused]] uint8_t receiverKind = readU1();
        [[maybe_unused]] int32_t receiverTypeCpIndex = readS4be();
        [[maybe_unused]] int32_t receiverNameCpIndex = readS4be();
    }

    auto taintTableLength = readS8be();
    is.ignore(taintTableLength);

    auto docLength = readS4be();
    is.ignore(docLength);

    int32_t depended_global_var_length = readS4be();
    for (auto i = 0; i < depended_global_var_length; i++) {
        [[maybe_unused]] int32_t depended_global_var_cp_entry = readS4be();
    }
    [[maybe_unused]] int64_t scopeTableLength = readS8be();
    int32_t scopeEntryCount = readS4be();
    // scope entry
    for (auto i = 0; i < scopeEntryCount; i++) {
        [[maybe_unused]] int32_t currentScopeIndex = readS4be();
        [[maybe_unused]] int32_t instructionOffset = readS4be();
        uint8_t hasParent = readU1();
        if (hasParent != 0U) {
            [[maybe_unused]] int32_t parentScopeIndex = readS4be();
        }
    }

    [[maybe_unused]] int64_t functionBodyLength = readS8be();
    // function body
    [[maybe_unused]] int32_t argsCount = readS4be();
    uint8_t hasReturnVar = readU1();

    // returnVar
    if (hasReturnVar != 0U) {
        uint8_t kind = readU1();
        int32_t typeCpIndex = readS4be();
        auto type = constantPool->getTypeCp(typeCpIndex, false);
        int32_t nameCpIndex = readS4be();
        birFunction.returnVar = Variable(std::move(type), constantPool->getStringCp(nameCpIndex), (VarKind)kind);
    }

    int32_t paramsWithDefaults = readS4be();
    birFunction.requiredParams.reserve(paramsWithDefaults);
    for (auto i = 0; i < paramsWithDefaults; i++) {
        uint8_t kind = readU1();
        int32_t typeCpIndex = readS4be();
        birFunction.requiredParams.emplace_back(std::move(functionParams.front()),
                                                constantPool->getTypeCp(typeCpIndex, false));
        functionParams.pop();
        [[maybe_unused]] int32_t nameCpIndex = readS4be();
        if (kind == ARG_VAR_KIND) {
            [[maybe_unused]] int32_t metaVarNameCpIndex = readS4be();
        }
        [[maybe_unused]] uint8_t hasDefaultExpr = readU1();
    }

    int32_t localVarCount = readS4be();
    birFunction.localVars.reserve(localVarCount);
    for (auto i = 0; i < localVarCount; i++) {
        readLocalVar(birFunction);
    }

    for (auto i = 0; i < paramsWithDefaults; i++) {
        // default parameter basic blocks info
        int32_t defaultParameterBBCount = readS4be();
        for (auto i = 0; i < defaultParameterBBCount; i++) {
            readBasicBlock(birFunction, true);
        }
    }

    // basic block info
    int32_t BBCount = readS4be();
    birFunction.basicBlocks.reserve(BBCount);
    for (auto i = 0; i < BBCount; i++) {
        readBasicBlock(birFunction);
    }

    // error table
    [[maybe_unused]] int32_t errorEntriesCount = readS4be();
    [[maybe_unused]] int32_t channelsLength = readS4be();

    if (ignore || ignoreFunction(functionName)) {
        package.functions.pop_back();
    }
}

StringCpInfo::StringCpInfo() { setTag(TAG_ENUM_CP_ENTRY_STRING); }

void StringCpInfo::read() {
    auto stringLength = readerRef.readS4be();
    std::unique_ptr<char[]> result(new char[stringLength]);
    readerRef.is.read(result.get(), stringLength);
    value = std::string(result.get(), stringLength);
}

ShapeCpInfo::ShapeCpInfo() { setTag(TAG_ENUM_CP_ENTRY_SHAPE); }

void ShapeCpInfo::read() {
    shapeLength = readerRef.readS4be();
    typeTag = TypeTag(readerRef.readU1());
    nameIndex = readerRef.readS4be();
    typeFlag = readerRef.readS8be();
    typeSpecialFlag = readerRef.readS4be();
    [[maybe_unused]] int32_t shapeTypeLengthInfo = shapeLength - 13;
    switch (typeTag) {
    case TYPE_TAG_INVOKABLE: {
        uint8_t isAnyFunction = readerRef.readU1();
        if (isAnyFunction == 0U) {
            paramCount = readerRef.readS4be();
            for (auto i = 0; i < paramCount; i++) {
                int32_t paramTypeCpIndex = readerRef.readS4be();
                addParam(paramTypeCpIndex);
            }
            hasRestType = readerRef.readU1();
            if (hasRestType != 0U) {
                restTypeIndex = readerRef.readS4be();
            }
            returnTypeIndex = readerRef.readS4be();
        }
        break;
    }
    case TYPE_TAG_MAP: {
        constraintTypeCpIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_ERROR: {
        [[maybe_unused]] int32_t pkgIdCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t errorTypeNameCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t detailTypeCpIndex = readerRef.readS4be();
        auto typeIds = std::make_unique<TypeId>();
        typeIds->read();
        break;
    }
    case TYPE_TAG_STREAM: {
        constraintTypeCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t hasErrorType = readerRef.readU1();
        if (hasErrorType != 0) {
            [[maybe_unused]] int32_t errorTypeCpIndex = readerRef.readS4be();
        }
        break;
    }
    case TYPE_TAG_TYPEDESC: {
        constraintTypeCpIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_PARAMETERIZED_TYPE: {
        [[maybe_unused]] int32_t paramValueTypeCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t paramIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_FUTURE: {
        constraintTypeCpIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_OBJECT: {
        [[maybe_unused]] uint8_t isObjectType = readerRef.readU1();
        [[maybe_unused]] int32_t pkdIdCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t nameCpIndex = readerRef.readS4be();
        [[maybe_unused]] uint8_t isAbstract = readerRef.readU1();
        [[maybe_unused]] uint8_t isClient = readerRef.readU1();
        int32_t objectFieldsCount = readerRef.readS4be();
        auto objectFields = std::vector<std::unique_ptr<ObjectField>>();
        objectFields.reserve(objectFieldsCount);
        for (auto i = 0; i < objectFieldsCount; i++) {
            auto objectField = std::make_unique<ObjectField>();
            objectField->read();
            objectFields.push_back(std::move(objectField));
        }
        uint8_t hasGeneratedInitFunction = readerRef.readU1();
        if (hasGeneratedInitFunction != 0U) {
            auto generatedInitFunction = std::make_unique<ObjectAttachedFunction>();
            generatedInitFunction->read();
        }
        uint8_t hasInitFunction = readerRef.readU1();
        if (hasInitFunction != 0U) {
            auto initFunction = std::make_unique<ObjectAttachedFunction>();
            initFunction->read();
        }
        int32_t objectAttachedFunctionsCount = readerRef.readS4be();
        auto objectAttachedFunctions = std::vector<std::unique_ptr<ObjectAttachedFunction>>();
        objectAttachedFunctions.reserve(objectAttachedFunctionsCount);
        for (auto i = 0; i < objectAttachedFunctionsCount; i++) {
            auto objectAttachedFunction = std::make_unique<ObjectAttachedFunction>();
            objectAttachedFunction->read();
            objectAttachedFunctions.push_back(std::move(objectAttachedFunction));
        }
        int32_t typeInclusionsCount = readerRef.readS4be();
        auto typeInclusionsCpIndex = std::vector<int32_t>();
        typeInclusionsCpIndex.reserve(typeInclusionsCount);
        for (auto i = 0; i < typeInclusionsCount; i++) {
            typeInclusionsCpIndex.push_back(readerRef.readS4be());
        }
        auto typeIds = std::make_unique<TypeId>();
        typeIds->read();
        break;
    }
    case TYPE_TAG_UNION: {
        [[maybe_unused]] uint8_t isCyclic = readerRef.readU1();
        uint8_t hasName = readerRef.readU1();
        if (hasName != 0U) {
            [[maybe_unused]] int32_t pkdIdCpIndex = readerRef.readS4be();
            [[maybe_unused]] int32_t nameCpIndex = readerRef.readS4be();
        }
        [[maybe_unused]] int32_t memberTypeCount = readerRef.readS4be();
        auto memberTypeCpIndex = std::vector<int32_t>();
        memberTypeCpIndex.reserve(memberTypeCount);
        for (auto i = 0; i < memberTypeCount; i++) {
            memberTypeCpIndex.push_back(readerRef.readS4be());
        }
        [[maybe_unused]] int32_t originalMemberTypeCount = readerRef.readS4be();
        auto originalMemberTypeCpIndex = std::vector<int32_t>();
        originalMemberTypeCpIndex.reserve(originalMemberTypeCount);
        for (auto i = 0; i < originalMemberTypeCount; i++) {
            originalMemberTypeCpIndex.push_back(readerRef.readS4be());
        }
        uint8_t isEnumType = readerRef.readU1();
        if (isEnumType != 0U) {
            [[maybe_unused]] int32_t pkgCpIndex = readerRef.readS4be();
            [[maybe_unused]] int32_t enumName = readerRef.readS4be();
            [[maybe_unused]] int32_t enumMemberSize = readerRef.readS4be();
            auto enumMembers = std::vector<int32_t>();
            enumMembers.reserve(enumMemberSize);
            for (auto i = 0; i < enumMemberSize; i++) {
                enumMembers.push_back(readerRef.readS4be());
            }
        }
        break;
    }
    case TYPE_TAG_TUPLE: {
        [[maybe_unused]] int32_t tupleTypesCount = readerRef.readS4be();
        auto tupleTypeCpIndex = std::vector<int32_t>();
        tupleTypeCpIndex.reserve(tupleTypesCount);
        for (auto i = 0; i < tupleTypesCount; i++) {
            tupleTypeCpIndex.push_back(readerRef.readS4be());
        }
        hasRestType = readerRef.readU1();
        if (hasRestType != 0U) {
            [[maybe_unused]] int32_t restTypeCpIndex = readerRef.readS4be();
        }
        break;
    }
    case TYPE_TAG_INTERSECTION: {
        [[maybe_unused]] int32_t constituentTypesCount = readerRef.readS4be();
        auto constituentTypeCpIndex = std::vector<int32_t>();
        constituentTypeCpIndex.reserve(constituentTypesCount);
        for (auto i = 0; i < constituentTypesCount; i++) {
            constituentTypeCpIndex.push_back(readerRef.readS4be());
        }
        [[maybe_unused]] int32_t effectiveTypeCount = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_XML: {
        constraintTypeCpIndex = readerRef.readS4be();
        break;
    }
    case TYPE_TAG_TABLE: {
        constraintTypeCpIndex = readerRef.readS4be();
        uint8_t hasFieldNameList = readerRef.readU1();
        if (hasFieldNameList != 0U) {
            auto fieldNameList = std::make_unique<TableFieldNameList>();
            fieldNameList->read();
        }
        uint8_t hasKeyConstraintType = readerRef.readU1();
        if (hasKeyConstraintType != 0U) {
            [[maybe_unused]] int32_t keyConstraintTypeCpIndex = readerRef.readS4be();
        }
        break;
    }
    case TYPE_TAG_RECORD: {
        [[maybe_unused]] int32_t pkdIdCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t nameCpIndex = readerRef.readS4be();
        [[maybe_unused]] uint8_t isSealed = readerRef.readU1();
        [[maybe_unused]] int32_t resetFieldTypeCpIndex = readerRef.readS4be();
        [[maybe_unused]] int32_t recordFieldCount = readerRef.readS4be();
        auto recordFields = std::vector<std::unique_ptr<RecordField>>();
        recordFields.reserve(recordFieldCount);
        for (auto i = 0; i < recordFieldCount; i++) {
            auto recordField = std::make_unique<RecordField>();
            recordField->read();
            recordFields.push_back(std::move(recordField));
        }
        [[maybe_unused]] int32_t hasInitFunction = readerRef.readU1();
        if (hasInitFunction != 0U) {
            auto recordInitFunction = std::make_unique<ObjectAttachedFunction>();
            recordInitFunction->read();
        }
        [[maybe_unused]] int32_t typeInclusionsCount = readerRef.readS4be();
        auto typeInclusionsCpIndex = std::vector<int32_t>();
        typeInclusionsCpIndex.reserve(typeInclusionsCount);
        for (auto i = 0; i < typeInclusionsCount; i++) {
            typeInclusionsCpIndex.push_back(readerRef.readS4be());
        }
        break;
    }
    case TYPE_TAG_FINITE: {
        [[maybe_unused]] int32_t nameCpIndex = readerRef.readS4be();
        [[maybe_unused]] int64_t flags = readerRef.readS8be();
        int32_t valueSpaceSize = readerRef.readS4be();
        for (auto i = 0; i < valueSpaceSize; i++) {
            [[maybe_unused]] int32_t fTypeCpIndex = readerRef.readS4be();
            int32_t valueLength = readerRef.readS4be();
            for (auto j = 0; j < valueLength; j++) {
                [[maybe_unused]] int8_t tmp = readerRef.readU1();
            }
        }

        break;
    }
    case TYPE_TAG_NIL:
    case TYPE_TAG_INT:
    case TYPE_TAG_BYTE:
    case TYPE_TAG_FLOAT:
    case TYPE_TAG_DECIMAL:
    case TYPE_TAG_STRING:
    case TYPE_TAG_BOOLEAN:
    case TYPE_TAG_JSON:
    case TYPE_TAG_ANYDATA:
    case TYPE_TAG_ANY:
    case TYPE_TAG_ENDPOINT:
    case TYPE_TAG_PACKAGE:
    case TYPE_TAG_NONE:
    case TYPE_TAG_VOID:
    case TYPE_TAG_XMLNS:
    case TYPE_TAG_ANNOTATION:
    case TYPE_TAG_SEMANTIC_ERROR:
    case TYPE_TAG_ITERATOR:
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
    case TYPE_TAG_NULL_SET: {
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
        auto tag = static_cast<ConstantPoolEntry::tagEnum>(readerRef.readU1());
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

std::shared_ptr<Package> BIRReader::readModule() {
    int32_t idCpIndex = readS4be();
    ConstantPoolEntry *poolEntry = constantPool->getEntry(idCpIndex);
    auto birPackage = std::make_shared<Package>();

    switch (poolEntry->getTag()) {
    case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
        auto *packageEntry = static_cast<PackageCpInfo *>(poolEntry);
        poolEntry = constantPool->getEntry(packageEntry->getOrgIndex());
        auto *stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->org = stringCp->getValue();

        poolEntry = constantPool->getEntry(packageEntry->getNameIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->name = stringCp->getValue();

        poolEntry = constantPool->getEntry(packageEntry->getVersionIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->version = stringCp->getValue();
        break;
    }
    default:
        break;
    }

    // The following three are read into unused variables so that the file
    // pointer advances to the data that we need next.
    int32_t importCount = 0;
    int32_t constCount = 0;
    int32_t typeDefinitionCount = 0;

    importCount = readS4be();
    int32_t importSize = importCount * 12; // package_cp_info is 12 bytes
    is.ignore(importSize);

    constCount = readS4be();
    for (auto i = 0; i < constCount; i++) {
        [[maybe_unused]] int32_t constNameCpIndex = readS4be();
        [[maybe_unused]] int64_t constFlags = readS8be();
        [[maybe_unused]] uint8_t constOrigin = readU1();
        is.ignore(20);
        int32_t markdownLength = readS4be();
        is.ignore(markdownLength);
        [[maybe_unused]] int32_t constTypeCpIndex = readS4be();
        int64_t constValueLength = readS8be();
        is.ignore(constValueLength);
    }

    typeDefinitionCount = readS4be();
    for (auto i = 0; i < typeDefinitionCount; i++) {
        is.ignore(20);
        [[maybe_unused]] int32_t tdNameCpIndex = readS4be();
        [[maybe_unused]] int64_t tdFlags = readS8be();
        [[maybe_unused]] uint8_t tdLabel = readU1();
        [[maybe_unused]] uint8_t tdOrigin = readU1();
        int32_t markdownLength = readS4be();
        is.ignore(markdownLength);
        int64_t annotationAttachmentsContentLength = readS8be();
        is.ignore(annotationAttachmentsContentLength);
        [[maybe_unused]] int32_t tdTypeCpIndex = readS4be();
    }

    int32_t globalVarCount = readS4be();
    birPackage->globalVars.reserve(globalVarCount);
    for (auto i = 0; i < globalVarCount; i++) {
        readGlobalVar(*birPackage);
    }

    int32_t typeDefinitionBodiesCount = readS4be();
    for (auto i = 0; i < typeDefinitionBodiesCount; i++) {
        int32_t attachedFunctionsCount = readS4be();
        for (auto j = 0; j < attachedFunctionsCount; j++) {
            readFunction(*birPackage, true);
        }
        int32_t referencedTypesCount = readS4be();
        for (auto j = 0; j < referencedTypesCount; j++) {
            [[maybe_unused]] int32_t referencedType = readS4be();
        }
    }

    int32_t functionCount = readS4be();
    birPackage->functions.reserve(functionCount);
    for (auto i = 0; i < functionCount; i++) {
        readFunction(*birPackage);
    }

    return birPackage;
}

std::shared_ptr<Package> BIRReader::deserialize() {
    // Read Constant Pool
    auto *constantPoolSet = new ConstantPoolSet();
    constantPoolSet->read();
    setConstantPool(constantPoolSet);

    // Read Module
    return readModule();
}

void TypeId::read() {
    primaryTypeIdCount = readerRef.readS4be();
    primaryTypeId = std::vector<std::unique_ptr<TypeIdSet>>();
    primaryTypeId.reserve(primaryTypeIdCount);
    for (auto i = 0; i < primaryTypeIdCount; i++) {
        auto typeIdSet = std::make_unique<TypeIdSet>();
        typeIdSet->read();
        primaryTypeId.push_back(std::move(typeIdSet));
    }
    secondaryTypeIdCount = readerRef.readS4be();
    secondaryTypeId = std::vector<std::unique_ptr<TypeIdSet>>();
    secondaryTypeId.reserve(secondaryTypeIdCount);
    for (auto i = 0; i < secondaryTypeIdCount; i++) {
        auto typeIdSet = std::make_unique<TypeIdSet>();
        typeIdSet->read();
        secondaryTypeId.push_back(std::move(typeIdSet));
    }
}

void TypeIdSet::read() {
    pkgIdCpIndex = readerRef.readS4be();
    typeIdNameCpIndex = readerRef.readS4be();
    isPublicId = readerRef.readU1();
}

void ObjectField::read() {
    nameCpIndex = readerRef.readS4be();
    flags = readerRef.readS8be();
    doc = std::make_unique<Markdown>();
    doc->read();
    typeCpIndex = readerRef.readS4be();
}

void Markdown::read() {
    int32_t length = readerRef.readS4be();
    for (auto i = 0; i < length; i++) {
        [[maybe_unused]] int32_t body = readerRef.readS4be();
    }
}

void ObjectAttachedFunction::read() {
    nameCpIndex = readerRef.readS4be();
    flags = readerRef.readS8be();
    typeCpIndex = readerRef.readS4be();
}

void TableFieldNameList::read() {
    size = readerRef.readS4be();
    fieldNameCpIndex = std::vector<int32_t>();
    fieldNameCpIndex.reserve(size);
    for (auto i = 0; i < size; i++) {
        fieldNameCpIndex.push_back(readerRef.readS4be());
    }
}

void RecordField::read() {
    nameCpIndex = readerRef.readS4be();
    flags = readerRef.readS8be();
    doc = std::make_unique<Markdown>();
    doc->read();
    typeCpIndex = readerRef.readS4be();
}

} // namespace nballerina
