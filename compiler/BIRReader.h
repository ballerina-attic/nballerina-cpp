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

#include "ArrayInstructions.h"
#include "BasicBlock.h"
#include "BinaryOpInsn.h"
#include "ConditionBrInsn.h"
#include "ConstantLoad.h"
#include "Function.h"
#include "FunctionCallInsn.h"
#include "FunctionParam.h"
#include "GoToInsn.h"
#include "InvocableType.h"
#include "MapInsns.h"
#include "MoveInsn.h"
#include "NonTerminatorInsn.h"
#include "Operand.h"
#include "Package.h"
#include "ReturnInsn.h"
#include "StructureInsn.h"
#include "TerminatorInsn.h"
#include "TypeCastInsn.h"
#include "TypeDescInsn.h"
#include "TypeTestInsn.h"
#include "Types.h"
#include "UnaryOpInsn.h"
#include "Variable.h"
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

class TypeIdSet{
  public:
    TypeIdSet(){};
    void read();
    ~TypeIdSet(){};
    BIRReader &readerRef = BIRReader::reader;
  private:
    uint32_t pkgIdCpIndex;
    uint32_t typeIdNameCpIndex;
    uint8_t isPublicId;

  public:
    uint32_t getPkgCpIndex(){return pkgIdCpIndex;}
    uint32_t getTypeIdNameCpIndex(){return typeIdNameCpIndex;}
    uint8_t getIsPublicId(){return isPublicId;}
};
class TypeId{
  public:
    TypeId(){};
    void read();
    ~TypeId(){};
    BIRReader &readerRef = BIRReader::reader;
  private:
    uint32_t primaryTypeIdCount;
    std::vector<std::unique_ptr<TypeIdSet>> primaryTypeId;
    uint32_t secondaryTypeIdCount;
    std::vector<std::unique_ptr<TypeIdSet>> secondaryTypeId;
  
  public:
    uint32_t getPrimaryTypeIdCount(){return primaryTypeIdCount;}
    uint32_t getSecondaryTypeIdCount(){return secondaryTypeIdCount;}
    TypeIdSet *getPrimaryTypeId(int index){return primaryTypeId[index].get();}
    TypeIdSet *getSecondaryTypeId(int index){return secondaryTypeId[index].get();}
};

class Markdown{
  public:
    Markdown(){};
    void read();
    ~Markdown(){};
    BIRReader &readerRef = BIRReader::reader;
};

class ObjectField{
  public:
    ObjectField(){};
    void read();
    ~ObjectField(){};
    BIRReader &readerRef = BIRReader::reader;
  private:
    uint32_t nameCpIndex;
    uint64_t flags;
    std::unique_ptr<Markdown> doc;
    uint32_t typeCpIndex;
  public:
    uint32_t getNameCpIndex(){return nameCpIndex;}
    uint64_t getFlags(){return flags;}
    Markdown *getDoc(){return doc.get();}
    uint64_t getTypeCpIndex(){return typeCpIndex;}
};



// identical for RecordInitFunction
class ObjectAttachedFunction{
  public:
    ObjectAttachedFunction(){};
    void read();
    ~ObjectAttachedFunction(){};
    BIRReader &readerRef = BIRReader::reader;
  private:
    uint32_t nameCpIndex;
    uint64_t flags;
    uint32_t typeCpIndex;
  public:
    uint32_t getNameCpIndex(){return nameCpIndex;}
    uint64_t getFlags(){return flags;}
    uint32_t getTypeCpIndex(){return typeCpIndex;}
};

class TableFieldNameList{
  public:
    TableFieldNameList(){};
    void read();
    ~TableFieldNameList(){};
    BIRReader &readerRef = BIRReader::reader;
  private:
    uint32_t size;
    std::vector<uint32_t> fieldNameCpIndex;
};

class RecordField{
  public:
    RecordField(){};
    void read();
    ~RecordField(){};
    BIRReader &readerRef = BIRReader::reader;
  private:
    uint32_t nameCpIndex;
    uint64_t flags;
    std::unique_ptr<Markdown> doc;
    uint32_t typeCpIndex;
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
    uint32_t size;
    uint32_t elementTypeCpIndex;
    uint32_t pkgIdCpIndex;
    uint32_t errorTypeNameCpIndex;
    uint32_t detailTypeCpIndex;
    std::unique_ptr<TypeId> typeIds;
    uint8_t isAnyFunction;
    uint8_t hasErrorType;
    uint32_t errorTypeCpIndex;
    uint32_t paramValueTypeCpIndex;
    uint32_t paramIndex;
    //todo : add getters
    uint8_t isObjectType;
    uint32_t nameCpIndex;
    uint8_t isAbstract;
    uint8_t isClient;
    uint32_t objectFieldsCount;
    std::vector<std::unique_ptr<ObjectField>> objectFields;
    uint8_t hasGeneratedInitFunction;
    std::unique_ptr<ObjectAttachedFunction> generatedInitFunction;
    uint8_t hasInitFunction;
    std::unique_ptr<ObjectAttachedFunction> initFunction;
    uint32_t objectAttachedFunctionsCount;
    std::vector<std::unique_ptr<ObjectAttachedFunction>> objectAttachedFunctions;
    uint32_t typeInclusionsCount;
    std::vector<uint32_t> typeInclusionsCpIndex;
    uint8_t isCyclic;
    uint8_t hasName;
    uint32_t memberTypeCount;
    std::vector<uint32_t> memberTypeCpIndex;
    uint32_t originalMemberTypeCount;
    std::vector<uint32_t> originalMemberTypeCpIndex;
    uint8_t isEnumType;
    uint32_t pkgCpIndex;
    uint32_t enumName;
    uint32_t enumMemberSize;
    std::vector<uint32_t> enumMembers;
    uint32_t tupleTypesCount;
    std::vector<uint32_t> tupleTypeCpIndex;
    uint32_t restTypeCpIndex;
    uint32_t constituentTypesCount;
    std::vector<uint32_t> constituentTypeCpIndex;
    uint32_t effectiveTypeCount;
    uint8_t hasFieldNameList;
    std::unique_ptr<TableFieldNameList> fieldNameList;
    uint8_t hasKeyConstraintType;
    uint32_t keyConstraintTypeCpIndex;
    uint8_t isSealed;
    uint32_t resetFieldTypeCpIndex;
    uint32_t recordFieldCount;
    std::vector<std::unique_ptr<RecordField>> recordFields;
    std::unique_ptr<ObjectAttachedFunction> recordInitFunction; //todo: mayhave to change this in the future
    uint32_t pkdIdCpIndex;

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
    uint32_t getSize() { return size; }
    uint32_t getElementTypeCpIndex() { return elementTypeCpIndex; }
    uint32_t getPkgIdCpIndex() { return pkgIdCpIndex; }
    uint32_t getErrorTypeNameCpIndex() { return errorTypeNameCpIndex; }
    uint32_t getDetailTypeCpIndex() { return detailTypeCpIndex; }
    TypeId *getTypeIds(){return typeIds.get();} 
    uint8_t getIsAnyFunction(){return isAnyFunction;}
    uint8_t getHasErrorType(){return hasErrorType;}
    uint32_t getErrorTypeCpIndex() { return errorTypeCpIndex; }
    uint32_t getParamValueTypeCpIndex() { return paramValueTypeCpIndex; }
    uint32_t getParamIndex() { return paramIndex; }

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
    void setSize(uint32_t s) { size = s; }
    void setElementTypeCpIndex(uint32_t i) { elementTypeCpIndex = i; }
    void setPkgIdCpIndex(uint32_t i) { pkgIdCpIndex = i; }
    void setErrorTypeNameCpIndex(uint32_t i) { errorTypeNameCpIndex = i; }
    void setDetailTypeCpIndex(uint32_t i) { detailTypeCpIndex = i; }
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
