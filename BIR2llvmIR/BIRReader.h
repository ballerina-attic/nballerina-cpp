#ifndef BIRREADER_H
#define BIRREADER_H

#include "BIR.h"
#include <fstream>

class ConstantPoolSet;
class ConstantPoolEntry;
class PackageCpInfo;
class ShapeCpInfo;
class StringCpInfo;
class BIRReader;

class ConstantPoolEntry {

public:
  enum tagEnum {
    TAG_ENUM_CP_ENTRY_INTEGER = 1,
    TAG_ENUM_CP_ENTRY_FLOAT = 2,
    TAG_ENUM_CP_ENTRY_BOOLEAN = 3,
    TAG_ENUM_CP_ENTRY_STRING = 4,
    TAG_ENUM_CP_ENTRY_PACKAGE = 5,
    TAG_ENUM_CP_ENTRY_BYTE = 6,
    TAG_ENUM_CP_ENTRY_SHAPE = 7
  };
  ConstantPoolEntry() {}
  virtual void read() {}

private:
  tagEnum tag;

public:
  tagEnum getTag() { return tag; }
  void setTag(tagEnum t) { tag = t; }
};

class StringCpInfo : public ConstantPoolEntry {

public:
  StringCpInfo() {}
  void read(BIRReader *reader);
  ~StringCpInfo();

private:
  std::string value;

public:
  std::string getValue() { return value; }
  void setValue(std::string str) { value = str; }
};

class ShapeCpInfo : public ConstantPoolEntry {

public:
  ShapeCpInfo() {}
  void read(BIRReader *reader);
  ~ShapeCpInfo();

private:
  uint32_t shapeLength;
  std::string value;
  TypeTagEnum typeTag;
  uint32_t nameIndex;
  uint32_t typeFlag;
  uint32_t typeSpecialFlag;
  uint32_t paramCount;
  uint8_t hasRestType;
  uint32_t returnTypeIndex;
  uint32_t restTypeIndex;
  vector<uint32_t> params;

public:
  uint32_t getShapeLength() { return shapeLength; }
  std::string getValue() { return value; }
  TypeTagEnum getTypeTag() { return typeTag; }
  uint32_t getNameIndex() { return nameIndex; }
  uint32_t getTypeFlag() { return typeFlag; }
  uint32_t getTypeSpecialFlag() { return typeSpecialFlag; }
  uint32_t getParamCount() { return paramCount; }
  uint8_t getRestType() { return hasRestType; }
  uint32_t getReturnTypeIndex() { return returnTypeIndex; }
  uint32_t getRestTypeIndex() { return restTypeIndex; }
  void addParam(uint32_t param) { params.push_back(param); }
  uint32_t getParam(uint32_t index) { return params[index]; }

  void setShapeLength(uint32_t s) { shapeLength = s; }
  void setValue(std::string v) { value = v; }
  void setTypeTag(TypeTagEnum t) { typeTag = t; }
  void setNameIndex(uint32_t n) { nameIndex = n; }
  void setTypeFlag(uint32_t t) { typeFlag = t; }
  void setTypeSpecialFlag(uint32_t t) { typeSpecialFlag = t; }
  void setParamCount(uint32_t p) { paramCount = p; }
  void setRestType(uint8_t r) { hasRestType = r; }
  void setReturnTypeIndex(uint32_t r) { returnTypeIndex = r; }
  void setRestTypeIndex(uint32_t r) { restTypeIndex = r; }
};

class PackageCpInfo : public ConstantPoolEntry {

public:
  PackageCpInfo() {
    orgIndex = 0;
    nameIndex = 0;
    versionIndex = 0;
  }
  void read(BIRReader *reader);
  ~PackageCpInfo();

private:
  uint32_t orgIndex;
  uint32_t nameIndex;
  uint32_t versionIndex;

public:
  uint32_t getOrgIndex() { return orgIndex; }
  uint32_t getNameIndex() { return nameIndex; }
  uint32_t getVersionIndex() { return versionIndex; }
  void setOrgIndex(uint32_t org) { orgIndex = org; }
  void setNameIndex(uint32_t name) { nameIndex = name; }
  void setVersionIndex(uint32_t version) { versionIndex = version; }
};

class IntCpInfo : public ConstantPoolEntry {

public:
  IntCpInfo() {}
  void read(BIRReader *reader);
  ~IntCpInfo();

private:
  uint64_t value;

public:
  uint64_t getValue() { return value; }
  void setValue(uint64_t v) { value = v; }
};

class BooleanCpInfo : public ConstantPoolEntry {

public:
  BooleanCpInfo() {}
  void read(BIRReader *reader);
  ~BooleanCpInfo();

private:
  uint8_t value;

public:
  uint8_t getValue() { return value; }
  void setValue(uint8_t v) { value = v; }
};

class FloatCpInfo : public ConstantPoolEntry {

public:
  FloatCpInfo() {}
  void read(BIRReader *reader);
  ~FloatCpInfo();

private:
  double value;

public:
  double getValue() { return value; }
  void setValue(double v) { value = v; }
};

class ByteCpInfo : public ConstantPoolEntry {

public:
  ByteCpInfo() {}
  void read(BIRReader *reader);
  ~ByteCpInfo();

private:
  uint32_t value;

public:
  uint32_t getValue() { return value; }
  void setValue(uint32_t v) { value = v; }
};

class ConstantPoolSet {

public:
  ConstantPoolSet() {}
  void read(BIRReader *reader);
  ~ConstantPoolSet();

private:
  std::vector<ConstantPoolEntry *> *poolEntries;

public:
  ConstantPoolEntry *getEntry(int index) { return (*poolEntries)[index]; }
  std::string getStringCp(uint32_t index);
  uint32_t getIntCp(uint32_t index);
  TypeDecl *getTypeCp(uint32_t index, bool voidToInt);
  TypeTagEnum getTypeTag(uint32_t index);
  InvokableType *getInvokableType(uint32_t index);
};

class BIRReader {
private:
  std::string fileName;
  std::ifstream is;
  BIRReader() { }

  ConstantPoolSet *constantPool;
  VarDecl* readGlobalVar();
  Operand *readOperand();
  VarDecl *readLocalVar();
  TypeDescInsn* readTypeDescInsn();
  StructureInsn* readStructureInsn();
  void readInsn(BIRFunction *birFunction, BIRBasicBlock *basicBlock);
  BIRBasicBlock *readBasicBlock(BIRFunction *birFunction);
  void patchInsn(vector<BIRBasicBlock *> basicBlocks);
  BIRFunction *readFunction();
  void readModule();

  // Read bytes functions
  uint8_t readU1();
  uint8_t peekReadU1();
  uint16_t readS2be();
  uint32_t readS4be();
  uint64_t readS8be();

public:
  BIRPackage birPackage;
  static BIRReader reader;
  static BIRReader& getInstance()
  {
    return reader;
  }
  void setFileStream(std::string FileName) {
    if (fileName == FileName)
      return;
    fileName = FileName;
    if(is.is_open())
      is.close();
    is.open(fileName, ifstream::binary);
  }
  std::string getFileName() { return fileName; }
  void deserialize();
  void setConstantPool(ConstantPoolSet *constantPoolSet) {
    constantPool = constantPoolSet;
  }
  void patchTypesToFuncParam();
  friend class ConstantPoolEntry;
  friend class ConstantPoolSet;
  friend class PackageCpInfo;
  friend class ShapeCpInfo;
  friend class StringCpInfo;
  friend class IntCpInfo;
  friend class FloatCpInfo;
  friend class BooleanCpInfo;
  friend class ByteCpInfo;
  friend class ReadCondBrInsn;
  friend class ReadBinaryInsn;
  friend class ReadUnaryInsn;
  friend class ReadConstLoadInsn;
  friend class ReadMoveInsn;
  friend class ReadFuncCallInsn;
  friend class ReadTypeDescInsn;
  friend class ReadStructureInsn;
  friend class ReadGoToInsn;
  friend class ReadReturnInsn;
  friend class ReadTypeCastInsn;
  friend class ReadTypeTestInsn;
};

class ReadInsn
{
public:
 BIRReader& readerRef = BIRReader::reader;
 ReadInsn() { }
 ~ReadInsn() { }
};

class ReadNonTerminatorInstruction : public ReadInsn
{
public:
  ReadNonTerminatorInstruction() {}
  ~ReadNonTerminatorInstruction() {}
  virtual NonTerminatorInsn* readNonTerminatorInsn() { return NULL; }
};

class ReadTerminatorInstruction : public ReadInsn
{
public:
  ReadTerminatorInstruction() {}
  ~ReadTerminatorInstruction() {}
  virtual TerminatorInsn* readTerminatorInsn() { return NULL; }
};

class ReadCondBrInsn : public ReadTerminatorInstruction
{
public:
  ReadCondBrInsn() {}
  ~ReadCondBrInsn() {}
  ConditionBrInsn* readTerminatorInsn();
};

class ReadFuncCallInsn : public ReadTerminatorInstruction
{
public:
  ReadFuncCallInsn() {}
  ~ReadFuncCallInsn() {}
  FunctionCallInsn* readTerminatorInsn();
};

class ReadGoToInsn : public ReadTerminatorInstruction
{
public:
  ReadGoToInsn() {}
  ~ReadGoToInsn() {}
  GoToInsn* readTerminatorInsn();
};

class ReadReturnInsn : public ReadTerminatorInstruction
{
public:
  ReadReturnInsn() {}
  ~ReadReturnInsn() {}
  ReturnInsn* readTerminatorInsn();
};

class ReadBinaryInsn : public ReadNonTerminatorInstruction
{
public:
  ReadBinaryInsn() {}
  ~ReadBinaryInsn() {}
  BinaryOpInsn* readNonTerminatorInsn();
};

class ReadUnaryInsn : public ReadNonTerminatorInstruction
{
public:
  ReadUnaryInsn() {}
  ~ReadUnaryInsn() {}
  UnaryOpInsn* readNonTerminatorInsn();
};

class ReadConstLoadInsn : public ReadNonTerminatorInstruction
{
public:
  ReadConstLoadInsn() {}
  ~ReadConstLoadInsn() {}
  ConstantLoadInsn* readNonTerminatorInsn();
};

class ReadMoveInsn : public ReadNonTerminatorInstruction
{
public:
  ReadMoveInsn() {}
  ~ReadMoveInsn() {}
  MoveInsn* readNonTerminatorInsn();
};

class ReadTypeDescInsn : public ReadNonTerminatorInstruction
{
public:
  ReadTypeDescInsn() {}
  ~ReadTypeDescInsn() {}
  TypeDescInsn* readNonTerminatorInsn();
};

class ReadStructureInsn : public ReadNonTerminatorInstruction
{
public:
  ReadStructureInsn() {}
  ~ReadStructureInsn() {}
  StructureInsn* readNonTerminatorInsn();
};

class ReadTypeCastInsn : public ReadNonTerminatorInstruction
{
public:
  ReadTypeCastInsn() {}
  ~ReadTypeCastInsn() {}
  TypeCastInsn* readNonTerminatorInsn();
};

class ReadTypeTestInsn : public ReadNonTerminatorInstruction
{
public:
  ReadTypeTestInsn() {}
  ~ReadTypeTestInsn() {}
  TypeTestInsn* readNonTerminatorInsn();
};

#endif // BIRREADER_H
