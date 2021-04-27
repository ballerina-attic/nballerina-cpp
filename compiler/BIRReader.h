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

#ifndef BIRREADER_H
#define BIRREADER_H

#include "bir/ArrayInstructions.h"
#include "bir/BasicBlock.h"
#include "bir/BinaryOpInsn.h"
#include "bir/ConditionBrInsn.h"
#include "bir/ConstantLoad.h"
#include "bir/Function.h"
#include "bir/FunctionCallInsn.h"
#include "bir/FunctionParam.h"
#include "bir/GoToInsn.h"
#include "bir/InvocableType.h"
#include "bir/MapInsns.h"
#include "bir/MoveInsn.h"
#include "interfaces/NonTerminatorInsn.h"
#include "bir/Operand.h"
#include "bir/Package.h"
#include "bir/ReturnInsn.h"
#include "bir/StructureInsn.h"
#include "interfaces/TerminatorInsn.h"
#include "bir/TypeCastInsn.h"
#include "bir/TypeDescInsn.h"
#include "bir/TypeTestInsn.h"
#include "bir/Types.h"
#include "bir/UnaryOpInsn.h"
#include "bir/Variable.h"
#include <fstream>

class ConstantPoolSet;
class ConstantPoolEntry;
class PackageCpInfo;
class ShapeCpInfo;
class StringCpInfo;
class BIRReader;

class BIRReader {
  private:
    std::string fileName;
    std::ifstream is;
    BIRReader() {}

    ConstantPoolSet *constantPool;
    nballerina::Variable readGlobalVar();
    nballerina::Operand readOperand();
    nballerina::Variable readLocalVar();
    nballerina::MapConstruct readMapConstructor();
    nballerina::TypeDescInsn *readTypeDescInsn();
    nballerina::StructureInsn *readStructureInsn();
    void readInsn(std::shared_ptr<nballerina::BasicBlock> basicBlock);
    std::shared_ptr<nballerina::BasicBlock> readBasicBlock(std::shared_ptr<nballerina::Function> birFunction);
    std::shared_ptr<nballerina::Function> readFunction(std::shared_ptr<nballerina::Package> birPackage);
    std::shared_ptr<nballerina::Package> readModule();

    // Read bytes functions
    uint8_t readU1();
    uint8_t peekReadU1();
    int16_t readS2be();
    int32_t readS4be();
    int64_t readS8be();
    double readS8bef();
    bool isLittleEndian();
    static bool ignoreFunction(std::string funcName);

  public:
    static BIRReader reader;
    static BIRReader &getInstance() { return reader; }
    void setFileStream(std::string FileName) {
        if (fileName == FileName)
            return;
        fileName = FileName;
        if (is.is_open())
            is.close();
        is.open(fileName, std::ifstream::binary);
    }
    std::string getFileName() { return fileName; }
    std::shared_ptr<nballerina::Package> deserialize();
    void setConstantPool(ConstantPoolSet *constantPoolSet) { constantPool = constantPoolSet; }
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
    friend class ReadArrayInsn;
    friend class ReadArrayStoreInsn;
    friend class ReadArrayLoadInsn;
    friend class ReadMapStoreInsn;
    friend class ReadMapLoadInsn;
    friend class TypeId;
    friend class TypeIdSet;
    friend class ObjectField;
    friend class Markdown;
    friend class ObjectAttachedFunction;
    friend class TableFieldNameList;
    friend class RecordField;
};

class TypeIdSet {
  public:
    TypeIdSet() = default;
    void read();
    ~TypeIdSet() = default;
    BIRReader &readerRef = BIRReader::reader;

  private:
    int32_t pkgIdCpIndex;
    int32_t typeIdNameCpIndex;
    uint8_t isPublicId;

  public:
    int32_t getPkgCpIndex() { return pkgIdCpIndex; }
    int32_t getTypeIdNameCpIndex() { return typeIdNameCpIndex; }
    uint8_t getIsPublicId() { return isPublicId; }
};
class TypeId {
  public:
    TypeId() = default;
    void read();
    ~TypeId() = default;
    BIRReader &readerRef = BIRReader::reader;

  private:
    int32_t primaryTypeIdCount;
    std::vector<std::unique_ptr<TypeIdSet>> primaryTypeId;
    int32_t secondaryTypeIdCount;
    std::vector<std::unique_ptr<TypeIdSet>> secondaryTypeId;

  public:
    int32_t getPrimaryTypeIdCount() { return primaryTypeIdCount; }
    int32_t getSecondaryTypeIdCount() { return secondaryTypeIdCount; }
    TypeIdSet *getPrimaryTypeId(int index) { return primaryTypeId[index].get(); }
    TypeIdSet *getSecondaryTypeId(int index) { return secondaryTypeId[index].get(); }
};

class Markdown {
  public:
    Markdown() = default;
    void read();
    ~Markdown() = default;
    BIRReader &readerRef = BIRReader::reader;
};

class ObjectField {
  public:
    ObjectField() = default;
    void read();
    ~ObjectField() = default;
    BIRReader &readerRef = BIRReader::reader;

  private:
    int32_t nameCpIndex;
    int64_t flags;
    std::unique_ptr<Markdown> doc;
    int32_t typeCpIndex;

  public:
    int32_t getNameCpIndex() { return nameCpIndex; }
    int64_t getFlags() { return flags; }
    Markdown *getDoc() { return doc.get(); }
    int64_t getTypeCpIndex() { return typeCpIndex; }
};

// identical for RecordInitFunction
class ObjectAttachedFunction {
  public:
    ObjectAttachedFunction() = default;
    void read();
    ~ObjectAttachedFunction() = default;
    BIRReader &readerRef = BIRReader::reader;

  private:
    int32_t nameCpIndex;
    int64_t flags;
    int32_t typeCpIndex;

  public:
    int32_t getNameCpIndex() { return nameCpIndex; }
    int64_t getFlags() { return flags; }
    int32_t getTypeCpIndex() { return typeCpIndex; }
};

class TableFieldNameList {
  public:
    TableFieldNameList(){};
    void read();
    ~TableFieldNameList(){};
    BIRReader &readerRef = BIRReader::reader;

  private:
    int32_t size;
    std::vector<int32_t> fieldNameCpIndex;
};

class RecordField {
  public:
    RecordField() = default;
    void read();
    ~RecordField() = default;
    BIRReader &readerRef = BIRReader::reader;

  private:
    int32_t nameCpIndex;
    int64_t flags;
    std::unique_ptr<Markdown> doc;
    int32_t typeCpIndex;
};
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
    ConstantPoolEntry() = default;
    virtual ~ConstantPoolEntry() = default;
    virtual void read() {}

  private:
    tagEnum tag;

  public:
    BIRReader &readerRef = BIRReader::reader;
    tagEnum getTag() { return tag; }
    void setTag(tagEnum t) { tag = t; }
};

class StringCpInfo : public ConstantPoolEntry {

  public:
    StringCpInfo();
    void read() override;
    ~StringCpInfo() override = default;

  private:
    std::string value;

  public:
    std::string getValue() { return value; }
    void setValue(std::string str) { value = str; }
};

class ShapeCpInfo : public ConstantPoolEntry {

  public:
    ShapeCpInfo();
    void read() override;
    ~ShapeCpInfo() override = default;

  private:
    int32_t shapeLength;
    std::string value;
    nballerina::TypeTag typeTag;
    int32_t nameIndex;
    int32_t typeFlag;
    int32_t typeSpecialFlag;
    int32_t paramCount;
    uint8_t hasRestType;
    int32_t returnTypeIndex;
    int32_t restTypeIndex;
    std::vector<int32_t> params;
    int32_t constraintTypeCpIndex;
    uint8_t state;
    int32_t size;
    int32_t elementTypeCpIndex;

  public:
    int32_t getShapeLength() { return shapeLength; }
    std::string getValue() { return value; }
    nballerina::TypeTag getTypeTag() { return typeTag; }
    int32_t getNameIndex() { return nameIndex; }
    int32_t getTypeFlag() { return typeFlag; }
    int32_t getTypeSpecialFlag() { return typeSpecialFlag; }
    int32_t getParamCount() { return paramCount; }
    uint8_t getRestType() { return hasRestType; }
    int32_t getReturnTypeIndex() { return returnTypeIndex; }
    int32_t getRestTypeIndex() { return restTypeIndex; }
    void addParam(int32_t param) { params.push_back(param); }
    int32_t getParam(int32_t index) { return params[index]; }
    int32_t getConstraintTypeCpIndex() { return constraintTypeCpIndex; }
    uint8_t getState() { return state; }
    int32_t getSize() { return size; }
    int32_t getElementTypeCpIndex() { return elementTypeCpIndex; }

    void setShapeLength(int32_t s) { shapeLength = s; }
    void setValue(std::string v) { value = v; }
    void setTypeTag(nballerina::TypeTag t) { typeTag = t; }
    void setNameIndex(int32_t n) { nameIndex = n; }
    void setTypeFlag(int32_t t) { typeFlag = t; }
    void setTypeSpecialFlag(int32_t t) { typeSpecialFlag = t; }
    void setParamCount(int32_t p) { paramCount = p; }
    void setRestType(uint8_t r) { hasRestType = r; }
    void setReturnTypeIndex(int32_t r) { returnTypeIndex = r; }
    void setRestTypeIndex(int32_t r) { restTypeIndex = r; }
    void setState(uint8_t s) { state = s; }
    void setSize(int32_t s) { size = s; }
    void setElementTypeCpIndex(int32_t i) { elementTypeCpIndex = i; }
};

class PackageCpInfo : public ConstantPoolEntry {

  public:
    PackageCpInfo();
    void read() override;
    ~PackageCpInfo() override = default;

  private:
    int32_t orgIndex;
    int32_t nameIndex;
    int32_t versionIndex;

  public:
    int32_t getOrgIndex() { return orgIndex; }
    int32_t getNameIndex() { return nameIndex; }
    int32_t getVersionIndex() { return versionIndex; }
    void setOrgIndex(int32_t org) { orgIndex = org; }
    void setNameIndex(int32_t name) { nameIndex = name; }
    void setVersionIndex(int32_t version) { versionIndex = version; }
};

class IntCpInfo : public ConstantPoolEntry {

  public:
    IntCpInfo();
    void read() override;
    ~IntCpInfo() override = default;

  private:
    int64_t value;

  public:
    int64_t getValue() { return value; }
    void setValue(int64_t v) { value = v; }
};

class BooleanCpInfo : public ConstantPoolEntry {

  public:
    BooleanCpInfo();
    void read() override;
    ~BooleanCpInfo() override = default;

  private:
    uint8_t value;

  public:
    uint8_t getValue() { return value; }
    void setValue(uint8_t v) { value = v; }
};

class FloatCpInfo : public ConstantPoolEntry {

  public:
    FloatCpInfo();
    void read() override;
    ~FloatCpInfo() override = default;

  private:
    double value;

  public:
    double getValue() { return value; }
    void setValue(double v) { value = v; }
};

class ByteCpInfo : public ConstantPoolEntry {

  public:
    ByteCpInfo();
    void read() override;
    ~ByteCpInfo() override = default;

  private:
    int32_t value;

  public:
    int32_t getValue() { return value; }
    void setValue(int32_t v) { value = v; }
};

class ConstantPoolSet {

  public:
    ConstantPoolSet() {}
    void read();
    ~ConstantPoolSet();

  private:
    std::vector<std::unique_ptr<ConstantPoolEntry>> poolEntries;

  public:
    BIRReader &readerRef = BIRReader::reader;
    ConstantPoolEntry *getEntry(int index) { return poolEntries[index].get(); }
    std::string getStringCp(int32_t index);
    int64_t getIntCp(int32_t index);
    nballerina::Type getTypeCp(int32_t index, bool voidToInt);
    double getFloatCp(int32_t index);
    bool getBooleanCp(int32_t index);
    nballerina::TypeTag getTypeTag(int32_t index);
    nballerina::InvocableType getInvocableType(int32_t index);
};

class ReadInsn {
  public:
    BIRReader &readerRef = BIRReader::reader;
    ReadInsn() {}
    ~ReadInsn() {}
};

class ReadNonTerminatorInstruction : public ReadInsn {
  public:
    ReadNonTerminatorInstruction() {}
    ~ReadNonTerminatorInstruction() {}
};

class ReadTerminatorInstruction : public ReadInsn {
  public:
    ReadTerminatorInstruction() {}
    ~ReadTerminatorInstruction() {}
};

class ReadCondBrInsn : public ReadTerminatorInstruction {
  public:
    static ReadCondBrInsn readCondBrInsn;
    ReadCondBrInsn() {}
    ~ReadCondBrInsn() {}
    std::unique_ptr<nballerina::ConditionBrInsn> readTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadFuncCallInsn : public ReadTerminatorInstruction {
  public:
    static ReadFuncCallInsn readFuncCallInsn;
    ReadFuncCallInsn() {}
    ~ReadFuncCallInsn() {}
    std::unique_ptr<nballerina::FunctionCallInsn> readTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadGoToInsn : public ReadTerminatorInstruction {
  public:
    static ReadGoToInsn readGoToInsn;
    ReadGoToInsn() {}
    ~ReadGoToInsn() {}
    std::unique_ptr<nballerina::GoToInsn> readTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadReturnInsn : public ReadTerminatorInstruction {
  public:
    static ReadReturnInsn readReturnInsn;
    ReadReturnInsn() {}
    ~ReadReturnInsn() {}
    std::unique_ptr<nballerina::ReturnInsn> readTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadBinaryInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadBinaryInsn readBinaryInsn;
    ReadBinaryInsn() {}
    ~ReadBinaryInsn() {}
    std::unique_ptr<nballerina::BinaryOpInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadUnaryInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadUnaryInsn readUnaryInsn;
    ReadUnaryInsn() {}
    ~ReadUnaryInsn() {}
    std::unique_ptr<nballerina::UnaryOpInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadConstLoadInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadConstLoadInsn readConstLoadInsn;
    ReadConstLoadInsn() {}
    ~ReadConstLoadInsn() {}
    std::unique_ptr<nballerina::ConstantLoadInsn>
    readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadMoveInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadMoveInsn readMoveInsn;
    ReadMoveInsn() {}
    ~ReadMoveInsn() {}
    std::unique_ptr<nballerina::MoveInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadTypeDescInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadTypeDescInsn readTypeDescInsn;
    ReadTypeDescInsn() {}
    ~ReadTypeDescInsn() {}
    std::unique_ptr<nballerina::TypeDescInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadStructureInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadStructureInsn readStructureInsn;
    ReadStructureInsn() {}
    ~ReadStructureInsn() {}
    std::unique_ptr<nballerina::StructureInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadTypeCastInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadTypeCastInsn readTypeCastInsn;
    ReadTypeCastInsn() {}
    ~ReadTypeCastInsn() {}
    std::unique_ptr<nballerina::TypeCastInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadTypeTestInsn : public ReadNonTerminatorInstruction {
  public:
    ReadTypeTestInsn() {}
    static ReadTypeTestInsn readTypeTestInsn;
    ~ReadTypeTestInsn() {}
    std::unique_ptr<nballerina::TypeTestInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadArrayInsn : public ReadNonTerminatorInstruction {
  public:
    ReadArrayInsn() {}
    static ReadArrayInsn readArrayInsn;
    ~ReadArrayInsn() {}
    std::unique_ptr<nballerina::ArrayInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadArrayStoreInsn : public ReadNonTerminatorInstruction {
  public:
    ReadArrayStoreInsn() {}
    static ReadArrayStoreInsn readArrayStoreInsn;
    ~ReadArrayStoreInsn() {}
    std::unique_ptr<nballerina::ArrayStoreInsn>
    readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadArrayLoadInsn : public ReadNonTerminatorInstruction {
  public:
    ReadArrayLoadInsn() {}
    static ReadArrayLoadInsn readArrayLoadInsn;
    ~ReadArrayLoadInsn() {}
    std::unique_ptr<nballerina::ArrayLoadInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadMapStoreInsn : public ReadNonTerminatorInstruction {
  public:
    ReadMapStoreInsn() {}
    static ReadMapStoreInsn readMapStoreInsn;
    ~ReadMapStoreInsn() {}
    std::unique_ptr<nballerina::MapStoreInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

class ReadMapLoadInsn : public ReadNonTerminatorInstruction {
  public:
    ReadMapLoadInsn() {}
    static ReadMapLoadInsn readMapLoadInsn;
    ~ReadMapLoadInsn() {}
    std::unique_ptr<nballerina::MapLoadInsn> readNonTerminatorInsn(std::shared_ptr<nballerina::BasicBlock> currentBB);
};

#endif // BIRREADER_H
