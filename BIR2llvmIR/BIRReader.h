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
  void read() {}

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
  typeTagEnum typeTag;
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
  typeTagEnum getTypeTag() { return typeTag; }
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
  void setTypeTag(typeTagEnum t) { typeTag = t; }
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
  int64_t value;

public:
  uint64_t getValue() { return value; }
  void setValue(int64_t v) { value = v; }
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
  std::vector<ConstantPoolEntry *> *getConstantPoolEntries() const {
    return poolEntries;
  }
  ConstantPoolEntry *getEntry(int index) { return (*poolEntries)[index]; }
  std::string getStringCp(uint32_t index);
  uint32_t getIntCp(uint32_t index);
  TypeDecl *getTypeCp(uint32_t index, bool voidToInt);
  typeTagEnum getTypeTag(uint32_t index);
  InvokableType *getInvokableType(uint32_t index);
};

class BIRReader {
private:
  std::string fileName;
  std::ifstream is;
  BIRReader() { }

  ConstantPoolSet *constantPool;
  //BIRPackage *birPackage;
  VarDecl* readGlobalVar();
  Operand *readOperand();
  VarDecl *readLocalVar();
  TypeDescInsn* readTypeDescInsn();
  StructureInsn* readStructureInsn();
  uint32_t readGotoInsn();
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
    fileName = FileName;
    if(is.is_open())
      is.close();
    is.open(fileName, ifstream::binary);
  }
  void setFileName(std::string FileName) { fileName = FileName; }
  std::string getFileName() { return fileName; }
  void deserialize();//BIRPackage *);
  void setConstantPool(ConstantPoolSet *constantPoolSet) {
    constantPool = constantPoolSet;
  }
  //void setBIRPackage(BIRPackage *birPackageReader) {
  //  birPackage = birPackageReader;
  //}
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
};

class Insn
{
public:
 BIRReader& readerRef = BIRReader::reader;
 Insn() { }
 ~Insn() { }
 virtual NonTerminatorInsn* readNonTerminatorInsn() { return NULL; };
 virtual TerminatorInsn* readTerminatorInsn() { return NULL; };
};

class NonTerminatorInstruction : public Insn
{
public:
  NonTerminatorInstruction() {}
  ~NonTerminatorInstruction() {}
  NonTerminatorInsn* readNonTerminatorInsn() { return NULL; }
};

class TerminatorInstruction : public Insn
{
public:
  TerminatorInstruction() {}
  ~TerminatorInstruction() {}
  TerminatorInsn* readTerminatorInsn() { return NULL; }
};

class ReadCondBrInsn : public TerminatorInstruction
{
public:
  ReadCondBrInsn() {}
  ~ReadCondBrInsn() {}
  ConditionBrInsn* readTerminatorInsn();
};

class ReadFuncCallInsn : public TerminatorInstruction
{
public:
  ReadFuncCallInsn() {}
  ~ReadFuncCallInsn() {}
  FunctionCallInsn* readTerminatorInsn();
};

class ReadGoToInsn : public TerminatorInstruction
{
public:
  ReadGoToInsn() {}
  ~ReadGoToInsn() {}
  GoToInsn* readTerminatorInsn();
};

class ReadReturnInsn : public TerminatorInstruction
{
public:
  ReadReturnInsn() {}
  ~ReadReturnInsn() {}
  ReturnInsn* readTerminatorInsn();
};

class ReadBinaryInsn : public NonTerminatorInstruction
{
public:
  ReadBinaryInsn() {}
  ~ReadBinaryInsn() {}
  BinaryOpInsn* readNonTerminatorInsn();
};

class ReadUnaryInsn : public NonTerminatorInstruction
{
public:
  ReadUnaryInsn() {}
  ~ReadUnaryInsn() {}
  UnaryOpInsn* readNonTerminatorInsn();
};

class ReadConstLoadInsn : public NonTerminatorInstruction
{
public:
  ReadConstLoadInsn() {}
  ~ReadConstLoadInsn() {}
  ConstantLoadInsn* readNonTerminatorInsn();
};

class ReadMoveInsn : public NonTerminatorInstruction
{
public:
  ReadMoveInsn() {}
  ~ReadMoveInsn() {}
  MoveInsn* readNonTerminatorInsn();
};

class ReadTypeDescInsn : public NonTerminatorInstruction
{
public:
  ReadTypeDescInsn() {}
  ~ReadTypeDescInsn() {}
  TypeDescInsn* readNonTerminatorInsn();
};

class ReadStructureInsn : public NonTerminatorInstruction
{
public:
  ReadStructureInsn() {}
  ~ReadStructureInsn() {}
  StructureInsn* readNonTerminatorInsn();
};

#endif // BIRREADER_H
