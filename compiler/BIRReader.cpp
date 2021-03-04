#include "BIRReader.h"
#include "BIR.h"
#ifdef unix
#include <libgen.h>
#else
#define __attribute__(unused)
#endif
#include <stdio.h>
#include <stdlib.h>
using namespace std;

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
  assert(poolEntry->getTag() ==
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING);
  StringCpInfo *stringCp = static_cast<StringCpInfo *>(poolEntry);
  return stringCp->getValue();
}

// Search string from the constant pool based on index
uint32_t ConstantPoolSet::getIntCp(uint32_t index) {
  ConstantPoolEntry *poolEntry = getEntry(index);
  assert(poolEntry->getTag() ==
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER);
  IntCpInfo *intCp = static_cast<IntCpInfo *>(poolEntry);
  return intCp->getValue();
}

// Search float from the constant pool based on index
float ConstantPoolSet::getFloatCp(uint32_t index) {
  ConstantPoolEntry *poolEntry = getEntry(index);
  assert(poolEntry->getTag() !=
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT);
  FloatCpInfo *floatCp = static_cast<FloatCpInfo *>(poolEntry);
  return floatCp->getValue();
}

// Search boolean from the constant pool based on index
bool ConstantPoolSet::getBooleanCp(uint32_t index) {
  ConstantPoolEntry *poolEntry = getEntry(index);
  assert(poolEntry->getTag() !=
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN);
  BooleanCpInfo *booleanCp = static_cast<BooleanCpInfo *>(poolEntry);
  return booleanCp->getValue();
}

// Search type from the constant pool based on index
TypeDecl *ConstantPoolSet::getTypeCp(uint32_t index, bool voidToInt) {
  ConstantPoolEntry *poolEntry = getEntry(index);

  assert(poolEntry->getTag() ==
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
  ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);

  std::string name = getStringCp(shapeCp->getNameIndex());
  // if name is empty, create a random name anon-<5-digits>
  if (name == "")
    name.append("anon-" + std::to_string(random() % 100000));

  int type = shapeCp->getTypeTag();

  // Handle voidToInt flag
  if (type == TYPE_TAG_NIL && voidToInt)
    return new TypeDecl(TYPE_TAG_INT, name, shapeCp->getTypeFlag());

  // Handle Map type
  if (type == TYPE_TAG_MAP) {
    ConstantPoolEntry *shapeEntry =
        getEntry(shapeCp->getConstraintTypeCpIndex());
    assert(shapeEntry->getTag() ==
           ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
    ShapeCpInfo *typeShapeCp = static_cast<ShapeCpInfo *>(shapeEntry);
    int memberType = typeShapeCp->getTypeTag();
    return new MapTypeDecl(type, name, shapeCp->getTypeFlag(), memberType);
  }

  // Default return
  return new TypeDecl(type, name, shapeCp->getTypeFlag());
}

// Get the Type tag from the constant pool based on the index passed
TypeTagEnum ConstantPoolSet::getTypeTag(uint32_t index) {
  ConstantPoolEntry *poolEntry = getEntry(index);
  assert(poolEntry->getTag() ==
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
  ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
  return shapeCp->getTypeTag();
}

// Search type from the constant pool based on index
InvokableType *ConstantPoolSet::getInvokableType(uint32_t index) {
  InvokableType *invokableType = new InvokableType();
  ConstantPoolEntry *poolEntry = getEntry(index);
  assert(poolEntry->getTag() ==
         ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE);
  ShapeCpInfo *shapeCp = static_cast<ShapeCpInfo *>(poolEntry);
  for (unsigned int i = 0; i < shapeCp->getParamCount(); i++) {
    TypeDecl *typeDecl = getTypeCp(shapeCp->getParam(i), false);
    invokableType->addParamType(typeDecl);
  }
  if (shapeCp->getRestType()) {
    TypeDecl *typeDecl = getTypeCp(shapeCp->getRestTypeIndex(), false);
    invokableType->setRestType(typeDecl);
  }
  TypeDecl *typeDecl = getTypeCp(shapeCp->getReturnTypeIndex(), false);
  invokableType->setReturnType(typeDecl);
  return invokableType;
}

// Read Global Variable and push it to BIRPackage
VarDecl *BIRReader::readGlobalVar() {
  VarDecl *varDecl = new VarDecl();
  uint8_t kind = readU1();
  varDecl->setVarKind((VarKind)kind);

  uint32_t varDclNameCpIndex = readS4be();
  varDecl->setVarName((constantPool->getStringCp(varDclNameCpIndex)));

  uint32_t flags __attribute__((unused)) = readS4be();
  uint32_t length __attribute__((unused)) = readS4be();
  uint8_t hasDoc = readU1();
  if (hasDoc) {
    uint32_t descriptionCpIndex __attribute__((unused)) = readS4be();
    uint32_t returnValueDescriptionCpIndex __attribute__((unused)) = readS4be();
    uint32_t parameterCount __attribute__((unused)) = readS4be();
  }
  uint32_t typeCpIndex = readS4be();
  TypeDecl *typeDecl = constantPool->getTypeCp(typeCpIndex, false);
  varDecl->setTypeDecl(typeDecl);
  birPackage.insertGlobalVar(varDecl);
  return varDecl;
}

VarDecl *BIRReader::readLocalVar() {
  VarDecl *varDecl = new VarDecl();
  uint8_t kind = readU1();
  varDecl->setVarKind((VarKind)kind);

  uint32_t typeCpIndex = readS4be();
  TypeDecl *typeDecl = constantPool->getTypeCp(typeCpIndex, false);
  varDecl->setTypeDecl(typeDecl);

  uint32_t nameCpIndex = readS4be();
  varDecl->setVarName(constantPool->getStringCp(nameCpIndex));

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

// Read Local Variable and return VarDecl pointer
Operand *BIRReader::readOperand() {
  VarDecl *varDecl = new VarDecl();
  uint8_t ignoredVar = readU1();
  varDecl->setIgnore((bool)ignoredVar);

  uint8_t kind = readU1();
  varDecl->setVarKind((VarKind)kind);

  uint8_t scope = readU1();
  varDecl->setVarScope((VarScope)scope);

  uint32_t varDclNameCpIndex = readS4be();
  varDecl->setVarName((constantPool->getStringCp(varDclNameCpIndex)));

  if (varDecl->getVarKind() == GLOBAL_VAR_KIND) {
    uint32_t packageIndex __attribute__((unused)) = readS4be();
    uint32_t typeCpIndex = readS4be();
    varDecl->setTypeDecl(constantPool->getTypeCp(typeCpIndex, false));
  }
  Operand *operand = new Operand(varDecl);
  return operand;
}

// Read TYPEDESC Insn
TypeDescInsn *ReadTypeDescInsn::readNonTerminatorInsn() {
  Operand *lhsOp = readerRef.readOperand();
  TypeDescInsn *typeDescInsn = new TypeDescInsn();
  typeDescInsn->setLhsOperand(lhsOp);
  uint32_t typeCpIndex __attribute__((unused)) = readerRef.readS4be();
  return typeDescInsn;
}

// Read STRUCTURE Insn
StructureInsn *ReadStructureInsn::readNonTerminatorInsn() {
  Operand *rhsOp = readerRef.readOperand();
  Operand *lhsOp = readerRef.readOperand();
  StructureInsn *structureInsn = new StructureInsn();
  structureInsn->setLhsOperand(lhsOp);
  structureInsn->setRhsOp(rhsOp);
  return structureInsn;
}

// Read CONST_LOAD Insn
ConstantLoadInsn *ReadConstLoadInsn::readNonTerminatorInsn() {
  ConstantLoadInsn *constantloadInsn = new ConstantLoadInsn();
  uint32_t typeCpIndex = readerRef.readS4be();
  TypeDecl *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);

  Operand *lhsOperand = readerRef.readOperand();
  lhsOperand->getVarDecl()->setTypeDecl(typeDecl);

  TypeTagEnum typeTag = readerRef.constantPool->getTypeTag(typeCpIndex);
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
    constantloadInsn->setIntValue(
        readerRef.constantPool->getIntCp(valueCpIndex), typeTag);
    break;
  }
  case TYPE_TAG_BOOLEAN: {
    uint8_t valueCpIndex = readerRef.readU1();
    constantloadInsn->setBoolValue(
        readerRef.constantPool->getBooleanCp(valueCpIndex), typeTag);
    break;
  }
  case TYPE_TAG_FLOAT: {
    uint32_t valueCpIndex = readerRef.readS4be();
    constantloadInsn->setFloatValue(
        readerRef.constantPool->getFloatCp(valueCpIndex), typeTag);
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
  constantloadInsn->setLhsOperand(lhsOperand);
  return constantloadInsn;
}

// Read Unary Operand
UnaryOpInsn *ReadUnaryInsn::readNonTerminatorInsn() {
  UnaryOpInsn *unaryOpInsn = new UnaryOpInsn();
  Operand *rhsOp = readerRef.readOperand();
  Operand *lhsOp = readerRef.readOperand();

  unaryOpInsn->setRhsOp(rhsOp);
  unaryOpInsn->setLhsOperand(lhsOp);
  return unaryOpInsn;
}

// Read Binary Operand
BinaryOpInsn *ReadBinaryInsn::readNonTerminatorInsn() {
  BinaryOpInsn *binaryOpInsn = new BinaryOpInsn();
  Operand *rhsOp1 = readerRef.readOperand();
  Operand *rhsOp2 = readerRef.readOperand();
  Operand *lhsOp = readerRef.readOperand();

  binaryOpInsn->setRhsOp1(rhsOp1);
  binaryOpInsn->setRhsOp2(rhsOp2);
  binaryOpInsn->setLhsOperand(lhsOp);
  return binaryOpInsn;
}

// Read BRANCH Insn
ConditionBrInsn *ReadCondBrInsn::readTerminatorInsn() {
  ConditionBrInsn *conditionBrInsn = new ConditionBrInsn();
  Operand *lhsOp = readerRef.readOperand();
  conditionBrInsn->setLhsOperand(lhsOp);
  uint32_t trueBbIdNameCpIndex = readerRef.readS4be();
  uint32_t falseBbIdNameCpIndex = readerRef.readS4be();

  BIRBasicBlock *trueDummybasicBlock = new BIRBasicBlock(
      readerRef.constantPool->getStringCp(trueBbIdNameCpIndex));
  conditionBrInsn->setIfThenBB(trueDummybasicBlock);

  BIRBasicBlock *falseDummybasicBlock = new BIRBasicBlock(
      readerRef.constantPool->getStringCp(falseBbIdNameCpIndex));
  conditionBrInsn->setElseBB(falseDummybasicBlock);

  conditionBrInsn->setPatchStatus(true);
  conditionBrInsn->setNextBB(NULL);
  return conditionBrInsn;
}

// Read MOV Insn
MoveInsn *ReadMoveInsn::readNonTerminatorInsn() {
  MoveInsn *moveInsn = new MoveInsn();
  Operand *rhsOp = readerRef.readOperand();
  Operand *lhsOp = readerRef.readOperand();

  moveInsn->setRhsOp(rhsOp);
  moveInsn->setLhsOperand(lhsOp);

  return moveInsn;
}

// Read Function Call
FunctionCallInsn *ReadFuncCallInsn::readTerminatorInsn() {
  FunctionCallInsn *functionCallInsn = new FunctionCallInsn();
  uint8_t isVirtual = readerRef.readU1();
  functionCallInsn->setIsVirtual((bool)isVirtual);

  uint32_t packageIndex __attribute__((unused)) = readerRef.readS4be();
  uint32_t callNameCpIndex = readerRef.readS4be();
  functionCallInsn->setFunctionName(
      readerRef.constantPool->getStringCp(callNameCpIndex));

  uint32_t argumentsCount = readerRef.readS4be();
  functionCallInsn->setArgCount(argumentsCount);
  for (unsigned int i = 0; i < argumentsCount; i++) {
    Operand *param = readerRef.readOperand();
    functionCallInsn->addArgumentToList(param);
  }
  uint8_t hasLhsOperand = readerRef.readU1();
  if (hasLhsOperand) {
    Operand *lhsOp = readerRef.readOperand();
    functionCallInsn->setLhsOperand(lhsOp);
  }
  uint32_t thenBbIdNameCpIndex = readerRef.readS4be();
  BIRBasicBlock *dummybasicBlock = new BIRBasicBlock(
      readerRef.constantPool->getStringCp(thenBbIdNameCpIndex));
  functionCallInsn->setNextBB(dummybasicBlock);
  functionCallInsn->setPatchStatus(true);
  return functionCallInsn;
}

// Read TypeCast Insn
TypeCastInsn *ReadTypeCastInsn::readNonTerminatorInsn() {
  TypeCastInsn *typeCastInsn = new TypeCastInsn();
  Operand *lhsOperand = readerRef.readOperand();
  typeCastInsn->setLhsOperand(lhsOperand);
  Operand *rhsOperand = readerRef.readOperand();
  typeCastInsn->setRhsOp(rhsOperand);

  uint32_t typeCpIndex = readerRef.readS4be();
  TypeDecl *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
  typeCastInsn->setTypeDecl(typeDecl);
  uint8_t isCheckTypes = readerRef.readU1();
  typeCastInsn->setTypesChecking((bool)isCheckTypes);
  return typeCastInsn;
}

// Read Type Test Insn
TypeTestInsn *ReadTypeTestInsn::readNonTerminatorInsn() {
  TypeTestInsn *typeTestInsn = new TypeTestInsn();
  uint32_t typeCpIndex = readerRef.readS4be();
  TypeDecl *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
  typeTestInsn->setTypeDecl(typeDecl);
  Operand *lhsOperand = readerRef.readOperand();
  typeTestInsn->setLhsOperand(lhsOperand);
  Operand *rhsOperand = readerRef.readOperand();
  typeTestInsn->setRhsOp(rhsOperand);
  return typeTestInsn;
}

// Read Array Insn
ArrayInsn *ReadArrayInsn::readNonTerminatorInsn() {
  ArrayInsn *arrayInsn = new ArrayInsn();
  uint32_t typeCpIndex = readerRef.readS4be();
  TypeDecl *typeDecl = readerRef.constantPool->getTypeCp(typeCpIndex, false);
  arrayInsn->setTypeDecl(typeDecl);
  Operand *lhsOperand = readerRef.readOperand();
  arrayInsn->setLhsOperand(lhsOperand);
  Operand *sizeOperand = readerRef.readOperand();
  arrayInsn->setSizeOp(sizeOperand);
  return arrayInsn;
}

// Read Array Store Insn
ArrayStoreInsn *ReadArrayStoreInsn::readNonTerminatorInsn() {
  ArrayStoreInsn *arrayStoreInsn = new ArrayStoreInsn();
  Operand *lhsOperand = readerRef.readOperand();
  arrayStoreInsn->setLhsOperand(lhsOperand);
  Operand *keyOperand = readerRef.readOperand();
  arrayStoreInsn->setKeyOp(keyOperand);
  Operand *rhsOperand = readerRef.readOperand();
  arrayStoreInsn->setRhsOp(rhsOperand);
  return arrayStoreInsn;
}

// Read Array Load Insn
ArrayLoadInsn *ReadArrayLoadInsn::readNonTerminatorInsn() {
  ArrayLoadInsn *arrayLoadInsn = new ArrayLoadInsn();
  uint8_t optionalFieldAccess = readerRef.readU1();
  arrayLoadInsn->setOptionalFieldAccess((bool)optionalFieldAccess);
  uint8_t fillingRead = readerRef.readU1();
  arrayLoadInsn->setFillingRead((bool)fillingRead);
  Operand *lhsOperand = readerRef.readOperand();
  arrayLoadInsn->setLhsOperand(lhsOperand);
  Operand *keyOperand = readerRef.readOperand();
  arrayLoadInsn->setKeyOp(keyOperand);
  Operand *rhsOperand = readerRef.readOperand();
  arrayLoadInsn->setRhsOp(rhsOperand);
  return arrayLoadInsn;
}

// Read Map Store Insn
MapStoreInsn *ReadMapStoreInsn::readNonTerminatorInsn() {
  MapStoreInsn *mapStoreInsn = new MapStoreInsn();
  Operand *lhsOperand = readerRef.readOperand();
  mapStoreInsn->setLhsOperand(lhsOperand);
  Operand *keyOperand = readerRef.readOperand();
  mapStoreInsn->setKeyOp(keyOperand);
  Operand *rhsOperand = readerRef.readOperand();
  mapStoreInsn->setRhsOp(rhsOperand);
  return mapStoreInsn;
}

GoToInsn *ReadGoToInsn::readTerminatorInsn() {
  GoToInsn *gotoInsn = new GoToInsn();
  uint32_t nameId = readerRef.readS4be();
  BIRBasicBlock *dummybasicBlock =
      new BIRBasicBlock(readerRef.constantPool->getStringCp(nameId));
  gotoInsn->setNextBB(dummybasicBlock);
  gotoInsn->setPatchStatus(true);
  return gotoInsn;
}

ReturnInsn *ReadReturnInsn::readTerminatorInsn() {
  ReturnInsn *returnInsn = new ReturnInsn();
  return returnInsn;
}

// Read an Instruction - either a NonTerminatorInsn or TerminatorInsn from the
// BIR
void BIRReader::readInsn(BIRFunction *birFunction, BIRBasicBlock *basicBlock) {
  NonTerminatorInsn *nonTerminatorInsn = new NonTerminatorInsn();
  nonTerminatorInsn->setFunction(birFunction);
  uint32_t sLine = readS4be();
  uint32_t eLine = readS4be();
  uint32_t sCol = readS4be();
  uint32_t eCol = readS4be();
  uint32_t sourceFileCpIndex = readS4be();

  Location *location =
      new Location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine,
                   (int)eLine, (int)sCol, (int)eCol);
  nonTerminatorInsn->setLocation(location);

  InstructionKind insnKind = (InstructionKind)readU1();
  switch (insnKind) {
  case INSTRUCTION_KIND_NEW_TYPEDESC: {
    TypeDescInsn *typeDescInsn =
        ReadTypeDescInsn::readTypeDescInsn.readNonTerminatorInsn();
    delete typeDescInsn;
    break;
  }
  case INSTRUCTION_KIND_NEW_STRUCTURE: {
    StructureInsn *structureInsn =
        ReadStructureInsn::readStructureInsn.readNonTerminatorInsn();
    structureInsn->setInstKind(insnKind);
    nonTerminatorInsn = structureInsn;
    break;
  }
  case INSTRUCTION_KIND_CONST_LOAD: {
    ConstantLoadInsn *constantloadInsn =
        ReadConstLoadInsn::readConstLoadInsn.readNonTerminatorInsn();
    constantloadInsn->setFunction(birFunction);
    constantloadInsn->setInstKind(insnKind);
    nonTerminatorInsn = constantloadInsn;
    break;
  }
  case INSTRUCTION_KIND_GOTO: {
    GoToInsn *gotoInsn = ReadGoToInsn::readGoToInsn.readTerminatorInsn();
    gotoInsn->setInstKind(insnKind);
    basicBlock->setTerminatorInsn(gotoInsn);
    nonTerminatorInsn = NULL;
    break;
  }
  case INSTRUCTION_KIND_RETURN: {
    ReturnInsn *returnInsn =
        ReadReturnInsn::readReturnInsn.readTerminatorInsn();
    returnInsn->setInstKind(insnKind);
    basicBlock->setTerminatorInsn(returnInsn);
    nonTerminatorInsn = NULL;
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
    BinaryOpInsn *binaryOpInsn =
        ReadBinaryInsn::readBinaryInsn.readNonTerminatorInsn();
    binaryOpInsn->setInstKind(insnKind);
    nonTerminatorInsn = binaryOpInsn;
    break;
  }
  case INSTRUCTION_KIND_UNARY_NEG:
  case INSTRUCTION_KIND_UNARY_NOT: {
    UnaryOpInsn *unaryOpInsn =
        ReadUnaryInsn::readUnaryInsn.readNonTerminatorInsn();
    unaryOpInsn->setInstKind(insnKind);
    nonTerminatorInsn = unaryOpInsn;
    break;
  }
  case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
    ConditionBrInsn *conditionBrInsn =
        ReadCondBrInsn::readCondBrInsn.readTerminatorInsn();
    conditionBrInsn->setInstKind(insnKind);
    basicBlock->setTerminatorInsn(conditionBrInsn);
    nonTerminatorInsn = NULL;
    break;
  }
  case INSTRUCTION_KIND_MOVE: {
    MoveInsn *moveInsn = ReadMoveInsn::readMoveInsn.readNonTerminatorInsn();
    moveInsn->setInstKind(insnKind);
    nonTerminatorInsn = (moveInsn);
    break;
  }
  case INSTRUCTION_KIND_CALL: {
    FunctionCallInsn *functionCallInsn =
        ReadFuncCallInsn::readFuncCallInsn.readTerminatorInsn();
    functionCallInsn->setInstKind(insnKind);
    basicBlock->setTerminatorInsn(functionCallInsn);
    nonTerminatorInsn = NULL;
    break;
  }
  case INSTRUCTION_KIND_TYPE_CAST: {
    TypeCastInsn *typeCastInsn =
        ReadTypeCastInsn::readTypeCastInsn.readNonTerminatorInsn();
    typeCastInsn->setInstKind(insnKind);
    nonTerminatorInsn = (typeCastInsn);
    break;
  }
  case INSTRUCTION_KIND_TYPE_TEST: {
    TypeTestInsn *typeTestInsn =
        ReadTypeTestInsn::readTypeTestInsn.readNonTerminatorInsn();
    typeTestInsn->setInstKind(insnKind);
    nonTerminatorInsn = (typeTestInsn);
    break;
  }
  case INSTRUCTION_KIND_NEW_ARRAY: {
    ArrayInsn *arrayInsn = ReadArrayInsn::readArrayInsn.readNonTerminatorInsn();
    arrayInsn->setInstKind(insnKind);
    nonTerminatorInsn = (arrayInsn);
    break;
  }
  case INSTRUCTION_KIND_ARRAY_STORE: {
    ArrayStoreInsn *arrayStoreInsn =
        ReadArrayStoreInsn::readArrayStoreInsn.readNonTerminatorInsn();
    arrayStoreInsn->setInstKind(insnKind);
    nonTerminatorInsn = (arrayStoreInsn);
    break;
  }
  case INSTRUCTION_KIND_ARRAY_LOAD: {
    ArrayLoadInsn *arrayLoadInsn =
        ReadArrayLoadInsn::readArrayLoadInsn.readNonTerminatorInsn();
    arrayLoadInsn->setInstKind(insnKind);
    nonTerminatorInsn = (arrayLoadInsn);
    break;
  }
  case INSTRUCTION_KIND_MAP_STORE: {
    MapStoreInsn *mapStoreInsn =
        ReadMapStoreInsn::readMapStoreInsn.readNonTerminatorInsn();
    mapStoreInsn->setInstKind(insnKind);
    nonTerminatorInsn = (mapStoreInsn);
    break;
  }
  default:
    fprintf(stderr, "%s:%d Invalid Insn Kind for Reader.\n", __FILE__,
            __LINE__);
    break;
  }
  if (nonTerminatorInsn)
    basicBlock->addNonTermInsn(nonTerminatorInsn);
}

// Read Basic Block from the BIR
BIRBasicBlock *BIRReader::readBasicBlock(BIRFunction *birFunction) {
  BIRBasicBlock *basicBlock = new BIRBasicBlock();
  uint32_t nameCpIndex = readS4be();
  basicBlock->setId(constantPool->getStringCp(nameCpIndex));
  basicBlock->setBIRFunction(birFunction);

  uint32_t insnCount = readS4be();
  for (unsigned int i = 0; i < insnCount; i++) {
    // Read an Instruction and adds it to basicBlock
    readInsn(birFunction, basicBlock);
  }
  return basicBlock;
}

// Patches the Terminator Insn with destination Basic Block
void BIRReader::patchInsn(vector<BIRBasicBlock *> basicBlocks) {
  for (unsigned int i = 0; i < basicBlocks.size(); i++) {
    BIRBasicBlock *basicBlock = basicBlocks[i];
    BIRFunction *curFunc = basicBlock->getBIRFunction();
    TerminatorInsn *terminator = basicBlock->getTerminatorInsn();
    if (terminator && terminator->getPatchStatus()) {
      switch (terminator->getInstKind()) {
      case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
        ConditionBrInsn *Terminator =
            (static_cast<ConditionBrInsn *>(terminator));
        BIRBasicBlock *trueBB =
            curFunc->searchBb(Terminator->getIfThenBB()->getId());
        BIRBasicBlock *falseBB =
            curFunc->searchBb(Terminator->getElseBB()->getId());
        BIRBasicBlock *danglingTrueBB = Terminator->getIfThenBB();
        BIRBasicBlock *danglingFalseBB = Terminator->getElseBB();
        delete danglingTrueBB;
        delete danglingFalseBB;
        Terminator->setIfThenBB(trueBB);
        Terminator->setElseBB(falseBB);
        Terminator->setPatchStatus(false);
        break;
      }
      case INSTRUCTION_KIND_GOTO: {
        BIRBasicBlock *destBB =
            curFunc->searchBb(terminator->getNextBB()->getId());
        BIRBasicBlock *danglingBB = terminator->getNextBB();
        delete danglingBB;
        terminator->setNextBB(destBB);
        terminator->setPatchStatus(false);
        break;
      }
      case INSTRUCTION_KIND_CALL: {
        BIRBasicBlock *destBB =
            curFunc->searchBb(terminator->getNextBB()->getId());
        BIRBasicBlock *danglingBB = terminator->getNextBB();
        delete danglingBB;
        terminator->setNextBB(destBB);
        break;
      }
      default:
        fprintf(stderr, "%s:%d Invalid Insn Kind for Instruction Patching.\n",
                __FILE__, __LINE__);
        break;
      }
    }
  }
}

// Reads BIR function
BIRFunction *BIRReader::readFunction() {
  BIRFunction *birFunction = new BIRFunction();
  uint32_t sLine = readS4be();
  uint32_t eLine = readS4be();
  uint32_t sCol = readS4be();
  uint32_t eCol = readS4be();
  uint32_t sourceFileCpIndex = readS4be();
  birPackage.setSrcFileName(constantPool->getStringCp(sourceFileCpIndex));

  Location *location =
      new Location(constantPool->getStringCp(sourceFileCpIndex), (int)sLine,
                   (int)eLine, (int)sCol, (int)eCol);
  birFunction->setLocation(location);

  uint32_t nameCpIndex = readS4be();
  birFunction->setName(constantPool->getStringCp(nameCpIndex));
  std::string initFuncName = "..<init>";
  std::string startFuncName = "..<start>";
  std::string stopFuncName = "..<stop>";
  if (!(birFunction->getName().rfind(initFuncName, 0) == 0 ||
        birFunction->getName().rfind(startFuncName, 0) == 0 ||
        birFunction->getName().rfind(stopFuncName, 0) == 0))
    birPackage.addFunctionLookUpEntry(birFunction->getName(), birFunction);

  uint32_t workdernameCpIndex = readS4be();
  birFunction->setWorkerName(constantPool->getStringCp(workdernameCpIndex));

  uint32_t flags = readS4be();
  birFunction->setFlags(flags);

  uint32_t typeCpIndex = readS4be();
  birFunction->setInvokableType(constantPool->getInvokableType(typeCpIndex));

  uint64_t annotationLength __attribute__((unused)) = readS8be();
  uint32_t annotationAttachments __attribute__((unused)) = readS4be();
  uint32_t requiredParamCount = readS4be();
  birFunction->setNumParams(requiredParamCount);
  // Set function param here and then fill remaining values from the default
  // Params
  for (unsigned int i = 0; i < requiredParamCount; i++) {
    uint32_t paramNameCpIndex = readS4be();
    VarDecl *varDecl = new VarDecl();
    varDecl->setVarName(constantPool->getStringCp(paramNameCpIndex));
    Operand *param = new Operand(varDecl);
    uint32_t paramFlags __attribute__((unused)) = readS4be();
    birFunction->setParam(param);
  }

  uint8_t hasRestParam = readU1();
  if (!hasRestParam)
    birFunction->setRestParam(NULL);

  uint8_t hasReceiver = readU1();
  if (!hasReceiver)
    birFunction->setReceiver(NULL);

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
    VarDecl *varDecl = new VarDecl();

    uint8_t kind = readU1();
    varDecl->setVarKind((VarKind)kind);

    uint32_t typeCpIndex = readS4be();
    TypeDecl *typeDecl = constantPool->getTypeCp(typeCpIndex, false);
    varDecl->setTypeDecl(typeDecl);

    uint32_t nameCpIndex = readS4be();
    varDecl->setVarName(constantPool->getStringCp(nameCpIndex));

    birFunction->setReturnVar(varDecl);
  }

  uint32_t defaultParamValue = readS4be();
  for (unsigned int i = 0; i < defaultParamValue; i++) {
    uint8_t kind = readU1();
    uint32_t typeCpIndex = readS4be();
    Operand *param = birFunction->getParam(i);
    param->getVarDecl()->setTypeDecl(
        constantPool->getTypeCp(typeCpIndex, false));
    param->getVarDecl()->setVarKind((VarKind)kind);
    uint32_t nameCpIndex __attribute__((unused)) = readS4be();
    if (kind == ARG_VAR_KIND) {
      uint32_t metaVarNameCpIndex __attribute__((unused)) = readS4be();
    }
    uint8_t hasDefaultExpr __attribute__((unused)) = readU1();
  }

  uint32_t localVarCount = readS4be();
  for (unsigned int i = 0; i < localVarCount; i++) {
    VarDecl *varDecl = readLocalVar();
    birFunction->insertLocalVar(varDecl);
  }
  for (unsigned int i = 0; i < defaultParamValue; i++) {
    uint32_t basicBlocksCount __attribute__((unused)) = readS4be();
  }

  uint32_t BBCount = readS4be();
  for (unsigned int i = 0; i < BBCount; i++) {
    BIRBasicBlock *basicBlock = readBasicBlock(birFunction);
    birFunction->addBIRBasicBlock(basicBlock);
    // Create links between the basic blocks
    if (i > 0) {
      BIRBasicBlock *prevBasicBlock = birFunction->getBasicBlock(i - 1);
      prevBasicBlock->setNextBB(basicBlock);
    }
  }

  // Patching the insn
  vector<BIRBasicBlock *> basicBlocks = birFunction->getBasicBlocks();
  patchInsn(basicBlocks);

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
  typeTag = static_cast<TypeTagEnum>(readerRef.readU1());
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
    fprintf(stderr, "%d is the Type Tag.\n", (TypeTagEnum)typeTag);
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
    ConstantPoolEntry::tagEnum tag =
        static_cast<ConstantPoolEntry::tagEnum>(readerRef.readU1());
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
void BIRReader::patchTypesToFuncParam() {
  for (size_t i = 0; i < birPackage.numFunctions(); i++) {
    BIRFunction *curFunc = birPackage.getFunction(i);
    for (size_t i = 0; i < curFunc->numBasicBlocks(); i++) {
      BIRBasicBlock *birBasicBlock = curFunc->getBasicBlock(i);
      for (size_t i = 0; i < birBasicBlock->numInsns(); i++) {
        TerminatorInsn *terminator = birBasicBlock->getTerminatorInsn();
        if (terminator && terminator->getPatchStatus()) {
          switch (terminator->getInstKind()) {
          case INSTRUCTION_KIND_CALL: {
            FunctionCallInsn *Terminator =
                (static_cast<FunctionCallInsn *>(terminator));
            for (int i = 0; i < Terminator->getArgCount(); i++) {
              BIRFunction *patchCallFunction =
                  birPackage.getFunctionLookUp(Terminator->getFunctionName());
              InvokableType *invokableType =
                  patchCallFunction->getInvokableType();
              for (size_t i = 0; i < invokableType->getParamTypeCount(); i++) {
                TypeDecl *typeDecl = invokableType->getParamType(i);
                Operand *param = Terminator->getArgumentFromList(i);
                param->getVarDecl()->setTypeDecl(typeDecl);
              }
            }
            break;
          }
          default:
            fprintf(stderr,
                    "%s:%d Invalid Insn Kind for assigning TypeDecl to "
                    "FuncCall Insn.\n",
                    __FILE__, __LINE__);
            break;
          }
        }
      }
    }
  }
}

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
      VarDecl *varDecl = readGlobalVar();
      birPackage.insertGlobalVar(varDecl);
    }
  }

  uint32_t typeDefinitionBodiesCount __attribute__((unused)) = readS4be();
  uint32_t functionCount = readS4be();

  // Push all the functions in BIRpackage except __init, __start & __stop
  std::string initFuncName = "..<init>";
  std::string startFuncName = "..<start>";
  std::string stopFuncName = "..<stop>";
  for (unsigned int i = 0; i < functionCount; i++) {
    BIRFunction *curFunc = readFunction();
    if (!(initFuncName.compare(curFunc->getName()) == 0 ||
          startFuncName.compare(curFunc->getName()) == 0 ||
          stopFuncName.compare(curFunc->getName()) == 0))
      birPackage.addFunction(curFunc);
    else
      delete curFunc;
  }

  uint32_t annotationsSize __attribute__((unused)) = readS4be();

  // Assign typedecl to function param of call Insn
  patchTypesToFuncParam();
}

void BIRReader::deserialize() {
  // Read Constant Pool
  ConstantPoolSet *constantPoolSet = new ConstantPoolSet();
  constantPoolSet->read();
  setConstantPool(constantPoolSet);

  // Read Module
  readModule();
}
