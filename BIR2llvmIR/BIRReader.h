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
  tagEnum getTag() const { return tag; }
};

class StringCpInfo : public ConstantPoolEntry {

public:
  StringCpInfo() {}
  void read(BIRReader *reader);
  ~StringCpInfo();

private:
  int32_t stringLength;
  std::string value;

public:
  int32_t getStringLength() const { return stringLength; }
  std::string getValue() const { return value; }
};

class ShapeCpInfo : public ConstantPoolEntry {

public:
  ShapeCpInfo() {}
  void read(BIRReader *reader);
  ~ShapeCpInfo();

private:
  int32_t shapeLength;
  std::string value;
  typeTagEnum typeTag;
  int32_t nameIndex;
  int32_t typeFlag;
  int32_t typeSpecialFlag;
  int32_t paramCount;
  uint8_t hasRestType;
  int32_t returnTypeIndex;
  int32_t restTypeIndex;
  vector<uint32_t> params;

public:
  int32_t getShapeLength() const { return shapeLength; }
  std::string getValue() const { return value; }
  typeTagEnum getTypeTag() const { return typeTag; }
  int32_t getNameIndex() const { return nameIndex; }
  int32_t getTypeFlag() const { return typeFlag; }
  int32_t getTypeSpecialFlag() const { return typeSpecialFlag; }
  int32_t getParamCount() const { return paramCount; }
  uint8_t getRestType() const { return hasRestType; }
  int32_t getReturnTypeIndex() const { return returnTypeIndex; }
  int32_t getRestTypeIndex() const { return restTypeIndex; }
  void addParam(uint32_t param) { params.push_back(param); }
  uint32_t getParam(uint32_t index) { return params[index]; }
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
  int32_t orgIndex;
  int32_t nameIndex;
  int32_t versionIndex;

public:
  int32_t getOrgIndex() const { return orgIndex; }
  int32_t getNameIndex() const { return nameIndex; }
  int32_t getVersionIndex() const { return versionIndex; }
};

class IntCpInfo : public ConstantPoolEntry {

public:
  IntCpInfo() {}
  void read(BIRReader *reader);
  ~IntCpInfo();

private:
  int64_t value;

public:
  int64_t getValue() const { return value; }
};

class BooleanCpInfo : public ConstantPoolEntry {

public:
  BooleanCpInfo() {}
  void read(BIRReader *reader);
  ~BooleanCpInfo();

private:
  uint8_t value;

public:
  uint8_t getValue() const { return value; }
};

class FloatCpInfo : public ConstantPoolEntry {

public:
  FloatCpInfo() {}
  void read(BIRReader *reader);
  ~FloatCpInfo();

private:
  double value;

public:
  double getValue() const { return value; }
};

class ByteCpInfo : public ConstantPoolEntry {

public:
  ByteCpInfo() {}
  void read(BIRReader *reader);
  ~ByteCpInfo();

private:
  int32_t value;

public:
  int32_t getValue() const { return value; }
};

class ConstantPoolSet {

public:
  ConstantPoolSet() {}
  void read(BIRReader *reader);
  ~ConstantPoolSet();

private:
  int32_t poolCount;
  std::vector<ConstantPoolEntry *> *poolEntries;

public:
  int32_t getConstantPoolCount() const { return poolCount; }
  std::vector<ConstantPoolEntry *> *getConstantPoolEntries() const {
    return poolEntries;
  }
  ConstantPoolEntry *getEntry(int index) { return (*poolEntries)[index]; }
  std::string getStringCp(int32_t index, ConstantPoolSet *constantPool);
  int32_t getIntCp(int32_t index, ConstantPoolSet *constantPool);
  TypeDecl *getTypeCp(int32_t index, ConstantPoolSet *constantPool,
                      bool voidToInt);
  typeTagEnum getTypeTag(int32_t index, ConstantPoolSet *constantPool);
  InvokableType *getInvokableType(int32_t index, ConstantPoolSet *constantPool);
};

class BIRReader {
private:
  std::string fileName;
  std::ifstream is;

  ConstantPoolSet *constantPool;
  BIRPackage *birPackage;
  void readGlobalVar();
  VarDecl *readLocalVar();
  Operand *readOperand();
  void readTypeDescInsn();
  void readStructureInsn();
  void readConstInsn(ConstantLoadInsn *constantloadInsn);
  uint32_t readGotoInsn();
  void readUnaryOpInsn(UnaryOpInsn *unaryOpInsn);
  void readBinaryOpInsn(BinaryOpInsn *binaryOpInsn);
  void readConditionalBrInsn(ConditionBrInsn *conditionBrInsn);
  void readMoveInsn(MoveInsn *moveInsn);
  void readFunctionCall(FunctionCallInsn *functionCallInsn);
  BIRBasicBlock *searchBb(vector<BIRBasicBlock *> basicBlocks,
                          std::string name);
  NonTerminatorInsn *readInsn(BIRFunction *birFunction,
                              BIRBasicBlock *basicBlock);
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
  BIRReader(std::string FileName) {
    fileName = FileName;
    is.open(fileName, ifstream::binary);
  }
  void setFileName(std::string FileName) { fileName = FileName; }
  std::string getFileName() { return fileName; }
  void deserialize(BIRPackage *);
  void setConstantPool(ConstantPoolSet *constantPoolSet) {
    constantPool = constantPoolSet;
  }
  void setBIRPackage(BIRPackage *birPackageReader) {
    birPackage = birPackageReader;
  }
  friend class ConstantPoolEntry;
  friend class ConstantPoolSet;
  friend class PackageCpInfo;
  friend class ShapeCpInfo;
  friend class StringCpInfo;
  friend class IntCpInfo;
  friend class FloatCpInfo;
  friend class BooleanCpInfo;
  friend class ByteCpInfo;
};

#endif // BIRREADER_H
