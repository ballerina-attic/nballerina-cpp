#ifndef BIRREADER_H
#define BIRREADER_H

#include "BIR.h"
#include <fstream>

class ConstantPoolSet;
class ConstantPoolEntry;
class PackageCpInfo;
class ShapeCpInfo;
class StringCpInfo;

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
  void read();

private:
  tagEnum tag;
  bool cpInfo;

public:
  tagEnum getTag() const { return tag; }
};

class StringCpInfo : public ConstantPoolEntry {

public:
  StringCpInfo() {}
  void read();
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
  void read();
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
  void read();
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
  void read();
  ~IntCpInfo();

private:
  int64_t value;

public:
  int64_t getValue() const { return value; }
};

class BooleanCpInfo : public ConstantPoolEntry {

public:
  BooleanCpInfo() {}
  void read();
  ~BooleanCpInfo();

private:
  uint8_t value;

public:
  uint8_t getValue() const { return value; }
};

class FloatCpInfo : public ConstantPoolEntry {

public:
  FloatCpInfo() {}
  void read();
  ~FloatCpInfo();

private:
  double value;

public:
  double getValue() const { return value; }
};

class ByteCpInfo : public ConstantPoolEntry {

public:
  ByteCpInfo() {}
  void read();
  ~ByteCpInfo();

private:
  int32_t value;

public:
  int32_t getValue() const { return value; }
};

class ConstantPoolSet {

public:
  ConstantPoolSet() {}
  void read();
  ~ConstantPoolSet();

private:
  int32_t poolCount;
  std::vector<ConstantPoolEntry *> *poolEntries;

public:
  int32_t getConstantPoolCount() const { return poolCount; }
  std::vector<ConstantPoolEntry *> *getConstantPoolEntries() const {
    return poolEntries;
  }
};

class BIRReader {
private:
  std::string fileName;

public:
  BIRReader(std::string FileName) { fileName = FileName; }
  void setFileName(std::string FileName) { fileName = FileName; }
  std::string getFileName() { return fileName; }
  void deserialize(BIRPackage *);
};

extern uint8_t readU1();
extern uint16_t readS2be();
extern uint32_t readS4be();
extern uint64_t readS8be();

#endif // BIRREADER_H
