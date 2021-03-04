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
#include <stdio.h>
#include <stdlib.h>

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
uint16_t BIRReader::readS2be() {
    uint16_t value;
    uint16_t result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    char *p = (char *)&value;
    char tmp;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    tmp = p[0];
    p[0] = p[1];
    p[1] = tmp;
#endif
    result = value;
    return result;
}

// Read 4 bytes from the stream
uint32_t BIRReader::readS4be() {
    uint32_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t result = 0;
    result |= (value & 0x000000FF) << 24;
    result |= (value & 0x0000FF00) << 8;
    result |= (value & 0x00FF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;
    value = result;
#endif
    return value;
}

// Read 8 bytes from the stream
uint64_t BIRReader::readS8be() {
    uint64_t value;
    uint64_t result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
#if __BYTE_ORDER == __LITTLE_ENDIAN
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
#endif
    result = value;
    return result;
}

// Search string from the constant pool based on index
std::string ConstantPoolSet::getStringCp(uint32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING);
    StringCpInfo *stringCp = static_cast<StringCpInfo *>(poolEntry);
    return stringCp->getValue();
}

// Search string from the constant pool based on index
uint32_t ConstantPoolSet::getIntCp(uint32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER);
    IntCpInfo *intCp = static_cast<IntCpInfo *>(poolEntry);
    return intCp->getValue();
}

// Search float from the constant pool based on index
float ConstantPoolSet::getFloatCp(uint32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() != ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT);
    FloatCpInfo *floatCp = static_cast<FloatCpInfo *>(poolEntry);
    return floatCp->getValue();
}

// Search boolean from the constant pool based on index
bool ConstantPoolSet::getBooleanCp(uint32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() != ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN);
    BooleanCpInfo *booleanCp = static_cast<BooleanCpInfo *>(poolEntry);
    return booleanCp->getValue();
}

// Search type from the constant pool based on index
Type *ConstantPoolSet::getTypeCp(uint32_t index, bool voidToInt) {
    ConstantPoolEntry *poolEntry = getEntry(index);

    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);

    std::string name = getStringCp(shapeCp->getNameIndex());
    // if name is empty, create a random name anon-<5-digits>
    if (name == "")
        name.append("anon-" + std::to_string(random() % 100000));

    TypeTag type = TypeTag(shapeCp->getTypeTag());

    // Handle voidToInt flag
    if (type == TYPE_TAG_NIL && voidToInt)
        return new Type(TYPE_TAG_INT, name, shapeCp->getTypeFlag());

    // Handle Map type
    if (type == TYPE_TAG_MAP) {
        ConstantPoolEntry *shapeEntry = getEntry(shapeCp->getConstraintTypeCpIndex());
        assert(shapeEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
        ShapeCpInfo *typeShapeCp = static_cast<ShapeCpInfo *>(shapeEntry);
        return new MapTypeDecl(type, name, shapeCp->getTypeFlag(), typeShapeCp->getTypeTag());
    }

    // Default return
    return new Type(type, name, shapeCp->getTypeFlag());
}

// Get the Type tag from the constant pool based on the index passed
TypeTag ConstantPoolSet::getTypeTag(uint32_t index) {
    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
    return shapeCp->getTypeTag();
}

// Search type from the constant pool based on index
InvokableType *ConstantPoolSet::getInvokableType(uint32_t index) {

    ConstantPoolEntry *poolEntry = getEntry(index);
    assert(poolEntry->getTag() == ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
    std::vector<Type *> paramTypes;
    for (unsigned int i = 0; i < shapeCp->getParamCount(); i++) {
        Type *typeDecl = getTypeCp(shapeCp->getParam(i), false);
        paramTypes.push_back(typeDecl);
    }
    Type *returnTypeDecl = getTypeCp(shapeCp->getReturnTypeIndex(), false);
    if (shapeCp->getRestType()) {
        Type *restTypeDecl = getTypeCp(shapeCp->getRestTypeIndex(), false);
        return new InvokableType(paramTypes, restTypeDecl, returnTypeDecl);
    }
    return new InvokableType(paramTypes, returnTypeDecl);
}

// Read Global Variable and push it to BIRPackage
Variable *BIRReader::readGlobalVar() {
    uint8_t kind = readU1();

    uint32_t varDclNameCpIndex = readS4be();

    uint32_t flags __attribute__((unused)) = readS4be();
    uint32_t length __attribute__((unused)) = readS4be();
    uint8_t hasDoc = readU1();
    if (hasDoc) {
        uint32_t descriptionCpIndex __attribute__((unused)) = readS4be();
        uint32_t returnValueDescriptionCpIndex __attribute__((unused)) = readS4be();
        uint32_t parameterCount __attribute__((unused)) = readS4be();
    }
    uint32_t typeCpIndex = readS4be();
    Type *typeDecl = constantPool->getTypeCp(typeCpIndex, false);
    Variable *varDecl = new Variable(typeDecl, (constantPool->getStringCp(varDclNameCpIndex)), (VarKind)kind);
    birPackage.insertGlobalVar(varDecl);
    return varDecl;
}

Variable *BIRReader::readLocalVar() {
    uint8_t kind = readU1();
    uint32_t typeCpIndex = readS4be();
    Type *typeDecl = constantPool->getTypeCp(typeCpIndex, false);
    uint32_t nameCpIndex = readS4be();

    Variable *varDecl = new Variable(typeDecl, constantPool->getStringCp(nameCpIndex), (VarKind)kind);

    if (kind == ARG_VAR_KIND) {
        uint32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
    } else if (kind == LOCAL_VAR_KIND) {
        uint32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
        uint32_t endBbIdCpIndex __attribute__((unused)) = readS4be();
        uint32_t startBbIdCpIndex __attribute__((unused)) = readS4be();
        uint32_t instructionOffset __attribute__((unused)) = readS4be();
    }
    return varDecl;
}

// Read Local Variable and return Variable pointer
Operand *BIRReader::readOperand() {
    uint8_t ignoredVar __attribute__((unused)) = readU1();

    uint8_t kind = readU1();

    uint8_t scope __attribute__((unused)) = readU1();

    uint32_t varDclNameCpIndex = readS4be();

    if ((VarKind)kind == GLOBAL_VAR_KIND) {
        uint32_t packageIndex __attribute__((unused)) = readS4be();
        uint32_t typeCpIndex = readS4be();
        Type *typeDecl __attribute__((unused)) = constantPool->getTypeCp(typeCpIndex, false);
    }

    Operand *operand = new Operand(constantPool->getStringCp(varDclNameCpIndex), (VarKind)kind);
    return operand;
}

// Read TYPEDESC Insn
TypeDescInsn *ReadTypeDescInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *lhsOp = readerRef.readOperand();
    uint32_t typeCpIndex __attribute__((unused)) = readerRef.readS4be();
    return new TypeDescInsn(lhsOp, currentBB);
}

// Read STRUCTURE Insn
StructureInsn *ReadStructureInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *rhsOp = readerRef.readOperand();
    Operand *lhsOp = readerRef.readOperand();
    return new StructureInsn(lhsOp, currentBB, rhsOp);
}

// Read CONST_LOAD Insn
ConstantLoadInsn *ReadConstLoadInsn::readNonTerminatorInsn(BasicBlock *currentBB) {

    uint32_t typeCpIndex __attribute__((unused)) = readerRef.readS4be();
    Operand *lhsOperand = readerRef.readOperand();
    ConstantLoadInsn *constantloadInsn = new ConstantLoadInsn(lhsOperand, currentBB);

    TypeTag typeTag = readerRef.constantPool->getTypeTag(typeCpIndex);
    switch (typeTag) {
    case TYPE_TAG_INT:
    case TYPE_TAG_UNSIGNED8_INT:
    case TYPE_TAG_UNSIGNED16_INT:
    case TYPE_TAG_UNSIGNED32_INT:
    case TYPE_TAG_SIGNED8_INT:
    case TYPE_TAG_SIGNED16_INT:
    case TYPE_TAG_SIGNED32_INT:
    case TYPE_TAG_DECIMAL:
    case TYPE_TAG_BYTE: {
        uint32_t valueCpIndex = readerRef.readS4be();
        constantloadInsn->setIntValue(readerRef.constantPool->getIntCp(valueCpIndex), typeTag);
        break;
    }
    case TYPE_TAG_BOOLEAN: {
        uint8_t valueCpIndex = readerRef.readU1();
        constantloadInsn->setBoolValue(readerRef.constantPool->getBooleanCp(valueCpIndex), typeTag);
        break;
    }
    case TYPE_TAG_FLOAT: {
        uint32_t valueCpIndex = readerRef.readS4be();
        constantloadInsn->setFloatValue(readerRef.constantPool->getFloatCp(valueCpIndex), typeTag);
        break;
    }
    case TYPE_TAG_CHAR_STRING:
    case TYPE_TAG_STRING: {
        uint32_t valueCpIndex = readerRef.readS4be();
        string *strVal = new string();
        strVal[0] = readerRef.constantPool->getStringCp(valueCpIndex);
        constantloadInsn->setStringValue(strVal, typeTag);
        break;
    }
    case TYPE_TAG_NIL:
        constantloadInsn->setTypeTagNil(typeTag);
    default:
        break;
    }
    return constantloadInsn;
}

// Read Unary Operand
UnaryOpInsn *ReadUnaryInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *rhsOp = readerRef.readOperand();
    Operand *lhsOp = readerRef.readOperand();
    return new UnaryOpInsn(lhsOp, currentBB, rhsOp);
}

// Read Binary Operand
BinaryOpInsn *ReadBinaryInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *rhsOp1 = readerRef.readOperand();
    Operand *rhsOp2 = readerRef.readOperand();
    Operand *lhsOp = readerRef.readOperand();
    return new BinaryOpInsn(lhsOp, currentBB, rhsOp1, rhsOp2);
}

// Read BRANCH Insn
ConditionBrInsn *ReadCondBrInsn::readTerminatorInsn(BasicBlock *currentBB) {
    Operand *lhsOp = readerRef.readOperand();
    uint32_t trueBbIdNameCpIndex = readerRef.readS4be();
    uint32_t falseBbIdNameCpIndex = readerRef.readS4be();

    BasicBlock *trueDummybasicBlock =
        new BasicBlock(readerRef.constantPool->getStringCp(trueBbIdNameCpIndex), currentBB->getFunction());

    BasicBlock *falseDummybasicBlock =
        new BasicBlock(readerRef.constantPool->getStringCp(falseBbIdNameCpIndex), currentBB->getFunction());

    return new ConditionBrInsn(lhsOp, currentBB, trueDummybasicBlock, falseDummybasicBlock);
}

// Read MOV Insn
MoveInsn *ReadMoveInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *rhsOp = readerRef.readOperand();
    Operand *lhsOp = readerRef.readOperand();

    return new MoveInsn(lhsOp, currentBB, rhsOp);
}

// Read Function Call
FunctionCallInsn *ReadFuncCallInsn::readTerminatorInsn(BasicBlock *currentBB) {
    uint8_t isVirtual = readerRef.readU1();
    uint32_t packageIndex __attribute__((unused)) = readerRef.readS4be();
    uint32_t callNameCpIndex = readerRef.readS4be();
    string funcName = readerRef.constantPool->getStringCp(callNameCpIndex);
    uint32_t argumentsCount = readerRef.readS4be();

    std::vector<Operand *> fnArgs;
    for (unsigned int i = 0; i < argumentsCount; i++) {
        Operand *param = readerRef.readOperand();
        fnArgs.push_back(param);
    }

    Operand *lhsOp = nullptr;
    uint8_t hasLhsOperand = readerRef.readU1();
    if (hasLhsOperand) {
        lhsOp = readerRef.readOperand();
    }
    uint32_t thenBbIdNameCpIndex = readerRef.readS4be();
    BasicBlock *dummybasicBlock =
        new BasicBlock(readerRef.constantPool->getStringCp(thenBbIdNameCpIndex), currentBB->getFunction());

    return new FunctionCallInsn((bool)isVirtual, funcName, argumentsCount, dummybasicBlock, lhsOp, fnArgs, currentBB);
}

// Read TypeCast Insn
TypeCastInsn *ReadTypeCastInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *lhsOperand = readerRef.readOperand();
    Operand *rhsOperand = readerRef.readOperand();

    uint32_t typeCpIndex = readerRef.readS4be();
    Type *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    uint8_t isCheckTypes = readerRef.readU1();

    return new TypeCastInsn(lhsOperand, currentBB, rhsOperand, typeDecl, (bool)isCheckTypes);
}

// Read Type Test Insn
TypeTestInsn *ReadTypeTestInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    uint32_t typeCpIndex = readerRef.readS4be();
    Type *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    Operand *lhsOperand = readerRef.readOperand();
    Operand *rhsOperand = readerRef.readOperand();
    return new TypeTestInsn(lhsOperand, currentBB, rhsOperand, typeDecl);
}

// Read Array Insn
ArrayInsn *ReadArrayInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    uint32_t typeCpIndex = readerRef.readS4be();
    Type *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
    Operand *lhsOperand = readerRef.readOperand();
    Operand *sizeOperand = readerRef.readOperand();
    return new ArrayInsn(lhsOperand, currentBB, sizeOperand, typeDecl);
}

// Read Array Store Insn
ArrayStoreInsn *ReadArrayStoreInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *lhsOperand = readerRef.readOperand();
    Operand *keyOperand = readerRef.readOperand();
    Operand *rhsOperand = readerRef.readOperand();
    return new ArrayStoreInsn(lhsOperand, currentBB, keyOperand, rhsOperand);
}

// Read Array Load Insn
ArrayLoadInsn *ReadArrayLoadInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    uint8_t optionalFieldAccess = readerRef.readU1();
    uint8_t fillingRead = readerRef.readU1();
    Operand *lhsOperand = readerRef.readOperand();
    Operand *keyOperand = readerRef.readOperand();
    Operand *rhsOperand = readerRef.readOperand();
    return new ArrayLoadInsn(lhsOperand, currentBB, (bool)optionalFieldAccess, (bool)fillingRead, keyOperand,
                             rhsOperand);
}

// Read Map Store Insn
MapStoreInsn *ReadMapStoreInsn::readNonTerminatorInsn(BasicBlock *currentBB) {
    Operand *lhsOperand = readerRef.readOperand();
    Operand *keyOperand = readerRef.readOperand();
    Operand *rhsOperand = readerRef.readOperand();
    return new MapStoreInsn(lhsOperand, currentBB, keyOperand, rhsOperand);
}

GoToInsn *ReadGoToInsn::readTerminatorInsn(BasicBlock *currentBB) {
    uint32_t nameId = readerRef.readS4be();
    BasicBlock *dummybasicBlock = new BasicBlock(readerRef.constantPool->getStringCp(nameId), currentBB->getFunction());
    return new GoToInsn(dummybasicBlock, currentBB);
}

ReturnInsn *ReadReturnInsn::readTerminatorInsn(BasicBlock *currentBB) { return new ReturnInsn(currentBB); }

// Read an Instruction - either a NonTerminatorInsn or TerminatorInsn from the
// BIR
void BIRReader::readInsn(BasicBlock *basicBlock) {

    uint32_t sLine = readS4be();
    uint32_t eLine = readS4be();
    uint32_t sCol = readS4be();
    uint32_t eCol = readS4be();
    uint32_t sourceFileCpIndex = readS4be();
    Location *location =
        new Location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    InstructionKind insnKind = (InstructionKind)readU1();

    switch (insnKind) {
    case INSTRUCTION_KIND_NEW_TYPEDESC: {
        TypeDescInsn *typeDescInsn = ReadTypeDescInsn::readTypeDescInsn.readNonTerminatorInsn(basicBlock);
        delete typeDescInsn;
        break;
    }
    case INSTRUCTION_KIND_NEW_STRUCTURE: {
        StructureInsn *structureInsn = ReadStructureInsn::readStructureInsn.readNonTerminatorInsn(basicBlock);
        structureInsn->setLocation(location);
        basicBlock->addNonTermInsn(structureInsn);
        break;
    }
    case INSTRUCTION_KIND_CONST_LOAD: {
        ConstantLoadInsn *constantloadInsn = ReadConstLoadInsn::readConstLoadInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(constantloadInsn);
        break;
    }
    case INSTRUCTION_KIND_GOTO: {
        GoToInsn *gotoInsn = ReadGoToInsn::readGoToInsn.readTerminatorInsn(basicBlock);
        basicBlock->setTerminatorInsn(gotoInsn);
        break;
    }
    case INSTRUCTION_KIND_RETURN: {
        ReturnInsn *returnInsn = ReadReturnInsn::readReturnInsn.readTerminatorInsn(basicBlock);
        basicBlock->setTerminatorInsn(returnInsn);
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
        BinaryOpInsn *binaryOpInsn = ReadBinaryInsn::readBinaryInsn.readNonTerminatorInsn(basicBlock);
        binaryOpInsn->setInstKind(insnKind);
        basicBlock->addNonTermInsn(binaryOpInsn);
        break;
    }
    case INSTRUCTION_KIND_UNARY_NEG:
    case INSTRUCTION_KIND_UNARY_NOT: {
        UnaryOpInsn *unaryOpInsn = ReadUnaryInsn::readUnaryInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(unaryOpInsn);
        break;
    }
    case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
        ConditionBrInsn *conditionBrInsn = ReadCondBrInsn::readCondBrInsn.readTerminatorInsn(basicBlock);
        basicBlock->setTerminatorInsn(conditionBrInsn);
        break;
    }
    case INSTRUCTION_KIND_MOVE: {
        MoveInsn *moveInsn = ReadMoveInsn::readMoveInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(moveInsn);
        break;
    }
    case INSTRUCTION_KIND_CALL: {
        FunctionCallInsn *functionCallInsn = ReadFuncCallInsn::readFuncCallInsn.readTerminatorInsn(basicBlock);
        basicBlock->setTerminatorInsn(functionCallInsn);
        break;
    }
    case INSTRUCTION_KIND_TYPE_CAST: {
        TypeCastInsn *typeCastInsn = ReadTypeCastInsn::readTypeCastInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(typeCastInsn);
        break;
    }
    case INSTRUCTION_KIND_TYPE_TEST: {
        TypeTestInsn *typeTestInsn = ReadTypeTestInsn::readTypeTestInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(typeTestInsn);
        break;
    }
    case INSTRUCTION_KIND_NEW_ARRAY: {
        ArrayInsn *arrayInsn = ReadArrayInsn::readArrayInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(arrayInsn);
        break;
    }
    case INSTRUCTION_KIND_ARRAY_STORE: {
        ArrayStoreInsn *arrayStoreInsn = ReadArrayStoreInsn::readArrayStoreInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(arrayStoreInsn);
        break;
    }
    case INSTRUCTION_KIND_ARRAY_LOAD: {
        ArrayLoadInsn *arrayLoadInsn = ReadArrayLoadInsn::readArrayLoadInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(arrayLoadInsn);
        break;
    }
    case INSTRUCTION_KIND_MAP_STORE: {
        MapStoreInsn *mapStoreInsn = ReadMapStoreInsn::readMapStoreInsn.readNonTerminatorInsn(basicBlock);
        basicBlock->addNonTermInsn(mapStoreInsn);
        break;
    }
    default:
        fprintf(stderr, "%s:%d Invalid Insn Kind for Reader.\n", __FILE__, __LINE__);
        break;
    }
}

// Read Basic Block from the BIR
BasicBlock *BIRReader::readBasicBlock(Function *birFunction) {
    uint32_t nameCpIndex = readS4be();
    BasicBlock *basicBlock = new BasicBlock(constantPool->getStringCp(nameCpIndex), birFunction);

    uint32_t insnCount = readS4be();
    for (size_t i = 0; i < insnCount; i++) {
        // Read an Instruction and adds it to basicBlock
        readInsn(basicBlock);
    }
    return basicBlock;
}

// Patches the Terminator Insn with destination Basic Block
void BIRReader::patchInsn(vector<BasicBlock *> basicBlocks) {
    for (auto const &basicBlock : basicBlocks) {
        Function *curFunc = basicBlock->getFunction();
        TerminatorInsn *terminator = basicBlock->getTerminatorInsn();
        if (terminator && terminator->getPatchStatus()) {
            switch (terminator->getInstKind()) {
            case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
                ConditionBrInsn *Terminator = (static_cast<ConditionBrInsn *>(terminator));
                BasicBlock *trueBB = curFunc->FindBasicBlock(Terminator->getIfThenBB()->getId());
                BasicBlock *falseBB = curFunc->FindBasicBlock(Terminator->getElseBB()->getId());
                BasicBlock *danglingTrueBB = Terminator->getIfThenBB();
                BasicBlock *danglingFalseBB = Terminator->getElseBB();
                delete danglingTrueBB;
                delete danglingFalseBB;
                Terminator->setIfThenBB(trueBB);
                Terminator->setElseBB(falseBB);
                Terminator->setPatched();
                break;
            }
            case INSTRUCTION_KIND_GOTO: {
                BasicBlock *destBB = curFunc->FindBasicBlock(terminator->getNextBB()->getId());
                BasicBlock *danglingBB = terminator->getNextBB();
                delete danglingBB;
                terminator->setNextBB(destBB);
                terminator->setPatched();
                break;
            }
            case INSTRUCTION_KIND_CALL: {
                BasicBlock *destBB = curFunc->FindBasicBlock(terminator->getNextBB()->getId());
                BasicBlock *danglingBB = terminator->getNextBB();
                delete danglingBB;
                terminator->setNextBB(destBB);
                break;
            }
            default:
                fprintf(stderr, "%s:%d Invalid Insn Kind for Instruction Patching.\n", __FILE__, __LINE__);
                break;
            }
        }
    }
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
Function *BIRReader::readFunction(Package *package) {

    // Read debug info
    uint32_t sLine = readS4be();
    uint32_t eLine = readS4be();
    uint32_t sCol = readS4be();
    uint32_t eCol = readS4be();
    uint32_t sourceFileCpIndex = readS4be();
    Location *location =
        new Location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    // TODO should not set src for every function
    package->setSrcFileName(constantPool->getStringCp(sourceFileCpIndex));

    uint32_t nameCpIndex = readS4be();
    std::string functionName = constantPool->getStringCp(nameCpIndex);
    uint32_t workdernameCpIndex = readS4be();
    uint32_t flags = readS4be();
    uint32_t typeCpIndex = readS4be();

    Function *birFunction = new Function(package, functionName, constantPool->getStringCp(workdernameCpIndex), flags,
                                         constantPool->getInvokableType(typeCpIndex));
    birFunction->setLocation(location);

    uint64_t annotationLength __attribute__((unused)) = readS8be();
    uint32_t annotationAttachments __attribute__((unused)) = readS4be();
    uint32_t requiredParamCount = readS4be();

    // Set function param here and then fill remaining values from the default
    // Params
    for (size_t i = 0; i < requiredParamCount; i++) {
        uint32_t paramNameCpIndex = readS4be();
        FunctionParam *param = new FunctionParam(constantPool->getStringCp(paramNameCpIndex));
        uint32_t paramFlags __attribute__((unused)) = readS4be();
        birFunction->insertParam(param);
    }

    uint8_t hasRestParam __attribute__((unused)) = readU1();
    // if (!hasRestParam)
    //   birFunction->setRestParam(NULL);

    uint8_t hasReceiver __attribute__((unused)) = readU1();
    // if (!hasReceiver)
    //   birFunction->setReceiver(NULL);

    uint64_t taintTableLength = readS8be();
    std::vector<char> taint(taintTableLength);
    is.read(&taint[0], taintTableLength);

    uint32_t docLength = readS4be();
    std::vector<char> doc(docLength);
    is.read(&doc[0], docLength);

    uint64_t functionBodyLength __attribute__((unused)) = readS8be();
    uint32_t argsCount __attribute__((unused)) = readS4be();
    uint8_t hasReturnVar = readU1();

    if (hasReturnVar) {
        uint8_t kind = readU1();
        uint32_t typeCpIndex = readS4be();
        Type *typeDecl = constantPool->getTypeCp(typeCpIndex, false);
        uint32_t nameCpIndex = readS4be();

        Variable *varDecl = new Variable(typeDecl, constantPool->getStringCp(nameCpIndex), (VarKind)kind);
        birFunction->setReturnVar(varDecl);
    }

    uint32_t defaultParamValue = readS4be();
    for (size_t i = 0; i < defaultParamValue; i++) {
        uint8_t kind = readU1();
        uint32_t typeCpIndex = readS4be();
        FunctionParam *param = birFunction->getParam(i);
        param->setType(constantPool->getTypeCp(typeCpIndex, false));

        uint32_t nameCpIndex __attribute__((unused)) = readS4be();
        if (kind == ARG_VAR_KIND) {
            uint32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
        }
        uint8_t hasDefaultExpr __attribute__((unused)) = readU1();
    }

    uint32_t localVarCount = readS4be();
    for (size_t i = 0; i < localVarCount; i++) {
        Variable *varDecl = readLocalVar();
        birFunction->insertLocalVar(varDecl);
    }
    // Ignore default param values
    for (size_t i = 0; i < defaultParamValue; i++) {
        uint32_t basicBlocksCount __attribute__((unused)) = readS4be();
    }

    uint32_t BBCount = readS4be();
    BasicBlock *previousBB = nullptr;
    for (size_t i = 0; i < BBCount; i++) {
        BasicBlock *basicBlock = readBasicBlock(birFunction);
        birFunction->insertBasicBlock(basicBlock);
        // Create links between the basic blocks
        if (previousBB) {
            previousBB->setNextBB(basicBlock);
        }
        previousBB = basicBlock;
    }

    // Patching the Instructions
    patchInsn(birFunction->getBasicBlocks());

    uint32_t errorEntriesCount __attribute__((unused)) = readS4be();
    uint32_t channelsLength __attribute__((unused)) = readS4be();
    return birFunction;
}

StringCpInfo::StringCpInfo() { setTag(TAG_ENUM_CP_ENTRY_STRING); }

void StringCpInfo::read() {
    uint32_t stringLength = readerRef.readS4be();
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

    uint32_t shapeLengthTypeInfo = shapeLength - 13;

    switch (typeTag) {
    case TYPE_TAG_INVOKABLE: {
        paramCount = readerRef.readS4be();
        for (unsigned int i = 0; i < paramCount; i++) {
            uint32_t paramTypeCpIndex = readerRef.readS4be();
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
    case TYPE_TAG_ARRAY:
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

void FloatCpInfo::read() { value = readerRef.readS8be(); }

ByteCpInfo::ByteCpInfo() { setTag(TAG_ENUM_CP_ENTRY_BYTE); }

void ByteCpInfo::read() { value = readerRef.readU1(); }

void ConstantPoolSet::read() {
    int constantPoolEntries = readerRef.readS4be();
    poolEntries = new std::vector<ConstantPoolEntry *>();
    poolEntries->reserve(constantPoolEntries);
    for (int i = 0; i < constantPoolEntries; i++) {
        ConstantPoolEntry::tagEnum tag = static_cast<ConstantPoolEntry::tagEnum>(readerRef.readU1());
        switch (tag) {
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
            PackageCpInfo *poolEntry = new PackageCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE: {
            ShapeCpInfo *poolEntry = new ShapeCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING: {
            StringCpInfo *poolEntry = new StringCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER: {
            IntCpInfo *poolEntry = new IntCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT: {
            FloatCpInfo *poolEntry = new FloatCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN: {
            BooleanCpInfo *poolEntry = new BooleanCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BYTE: {
            ByteCpInfo *poolEntry = new ByteCpInfo();
            poolEntry->read();
            poolEntries->push_back(poolEntry);
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
        if (terminator && terminator->getPatchStatus()) {
          switch (terminator->getInstKind()) {
          case INSTRUCTION_KIND_CALL: {
            FunctionCallInsn *Terminator =
                (static_cast<FunctionCallInsn *>(terminator));
            for (int i = 0; i < Terminator->getArgCount(); i++) {
              Function *patchCallFunction =
                  birPackage.getFunction(Terminator->getFunctionName());
              InvokableType *invokableType =
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

void BIRReader::readModule() {
    uint32_t idCpIndex = readS4be();
    ConstantPoolEntry *poolEntry = constantPool->getEntry(idCpIndex);
    switch (poolEntry->getTag()) {
    case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
        PackageCpInfo *packageEntry = static_cast<PackageCpInfo *>(poolEntry);
        poolEntry = constantPool->getEntry(packageEntry->getOrgIndex());
        StringCpInfo *stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage.setOrgName(stringCp->getValue());

        poolEntry = constantPool->getEntry(packageEntry->getNameIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage.setPackageName(stringCp->getValue());

        poolEntry = constantPool->getEntry(packageEntry->getVersionIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage.setVersion(stringCp->getValue());
        break;
    }
    default:
        break;
    }

    // The following three are read into unused variables so that the file
    // pointer advances to the data that we need next.
    uint32_t importCount __attribute__((unused));
    uint32_t constCount __attribute__((unused));
    uint32_t typeDefinitionCount __attribute__((unused));

    importCount = readS4be();
    constCount = readS4be();
    typeDefinitionCount = readS4be();

    uint32_t globalVarCount = readS4be();
    if (globalVarCount > 0) {
        for (unsigned int i = 0; i < globalVarCount; i++) {
            Variable *varDecl = readGlobalVar();
            birPackage.insertGlobalVar(varDecl);
        }
    }

    uint32_t typeDefinitionBodiesCount __attribute__((unused)) = readS4be();
    uint32_t functionCount = readS4be();

    // Push all the functions in BIRpackage except __init, __start & __stop
    for (unsigned int i = 0; i < functionCount; i++) {
        Function *curFunc = readFunction(&birPackage);
        if (ignoreFunction(curFunc->getName())) {
            delete curFunc;
        } else {
            birPackage.insertFunction(curFunc);
        }
    }

    uint32_t annotationsSize __attribute__((unused)) = readS4be();

    // Assign typedecl to function param of call Insn
    // patchTypesToFuncParam();
}

void BIRReader::deserialize() {
    // Read Constant Pool
    ConstantPoolSet *constantPoolSet = new ConstantPoolSet();
    constantPoolSet->read();
    setConstantPool(constantPoolSet);

    // Read Module
    readModule();
}
