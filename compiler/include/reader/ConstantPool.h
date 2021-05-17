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

#ifndef __CONSTANTPOOL__H__
#define __CONSTANTPOOL__H__

#include "bir/InvocableType.h"
#include "bir/Types.h"
#include "interfaces/Parser.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nballerina {

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
    virtual ~ConstantPoolEntry() = default;
    virtual void read(Parser &reader) = 0;

  private:
    tagEnum tag;

  public:
    tagEnum getTag() { return tag; }
    void setTag(tagEnum t) { tag = t; }
};

class StringCpInfo : public ConstantPoolEntry {

  public:
    StringCpInfo();
    void read(Parser &reader) override;

  private:
    std::string value;

  public:
    std::string getValue() { return value; }
    void setValue(std::string str) { value = str; }
};

class ShapeCpInfo : public ConstantPoolEntry {

  public:
    ShapeCpInfo();
    void read(Parser &reader) override;

  private:
    int32_t shapeLength;
    std::string value;
    TypeTag typeTag;
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
    TypeTag getTypeTag() { return typeTag; }
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
    void setTypeTag(TypeTag t) { typeTag = t; }
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
    void read(Parser &reader) override;

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
    void read(Parser &reader) override;

  private:
    int64_t value;

  public:
    int64_t getValue() { return value; }
    void setValue(int64_t v) { value = v; }
};

class BooleanCpInfo : public ConstantPoolEntry {

  public:
    BooleanCpInfo();
    void read(Parser &reader) override;

  private:
    uint8_t value;

  public:
    uint8_t getValue() { return value; }
    void setValue(uint8_t v) { value = v; }
};

class FloatCpInfo : public ConstantPoolEntry {

  public:
    FloatCpInfo();
    void read(Parser &reader) override;

  private:
    double value;

  public:
    double getValue() { return value; }
    void setValue(double v) { value = v; }
};

class ByteCpInfo : public ConstantPoolEntry {

  public:
    ByteCpInfo();
    void read(Parser &reader) override;

  private:
    int32_t value;

  public:
    int32_t getValue() { return value; }
    void setValue(int32_t v) { value = v; }
};

class ConstantPoolSet {

  public:
    void read(Parser &reader);

  private:
    std::vector<std::unique_ptr<ConstantPoolEntry>> poolEntries;

  public:
    ConstantPoolEntry *getEntry(int index) { return poolEntries[index].get(); }
    std::string getStringCp(int32_t index);
    int64_t getIntCp(int32_t index);
    int32_t getByteCp(int32_t index);
    Type getTypeCp(int32_t index, bool voidToInt);
    double getFloatCp(int32_t index);
    bool getBooleanCp(int32_t index);
    TypeTag getTypeTag(int32_t index);
    InvocableType getInvocableType(int32_t index);
};

class TypeIdSet {
  public:
    void read(Parser &reader);

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
    void read(Parser &reader);

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
    static void read(Parser &reader);
};

class ObjectField {
  public:
    void read(Parser &reader);

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
    void read(Parser &reader);

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
    void read(Parser &reader);

  private:
    int32_t size;
    std::vector<int32_t> fieldNameCpIndex;
};

class RecordField {
  public:
    void read(Parser &reader);

  private:
    int32_t nameCpIndex;
    int64_t flags;
    std::unique_ptr<Markdown> doc;
    int32_t typeCpIndex;
};

} // namespace nballerina

#endif //!__CONSTANTPOOL__H__
