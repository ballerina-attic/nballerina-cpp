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

#include "reader/ConstantPool.h"
#include <cassert>
#include <iostream>

namespace nballerina {

void TypeId::read(Parser &reader) {
    primaryTypeIdCount = reader.readS4be();
    primaryTypeId = std::vector<std::unique_ptr<TypeIdSet>>();
    primaryTypeId.reserve(primaryTypeIdCount);
    for (auto i = 0; i < primaryTypeIdCount; i++) {
        auto typeIdSet = std::make_unique<TypeIdSet>();
        typeIdSet->read(reader);
        primaryTypeId.push_back(std::move(typeIdSet));
    }
    secondaryTypeIdCount = reader.readS4be();
    secondaryTypeId = std::vector<std::unique_ptr<TypeIdSet>>();
    secondaryTypeId.reserve(secondaryTypeIdCount);
    for (auto i = 0; i < secondaryTypeIdCount; i++) {
        auto typeIdSet = std::make_unique<TypeIdSet>();
        typeIdSet->read(reader);
        secondaryTypeId.push_back(std::move(typeIdSet));
    }
}

void TypeIdSet::read(Parser &reader) {
    pkgIdCpIndex = reader.readS4be();
    typeIdNameCpIndex = reader.readS4be();
    isPublicId = reader.readU1();
}

void ObjectField::read(Parser &reader) {
    nameCpIndex = reader.readS4be();
    flags = reader.readS8be();
    doc = std::make_unique<Markdown>();
    doc->read(reader);
    typeCpIndex = reader.readS4be();
}

void Markdown::read(Parser &reader) {
    int32_t length = reader.readS4be();
    for (auto i = 0; i < length; i++) {
        [[maybe_unused]] int32_t body = reader.readS4be();
    }
}

void ObjectAttachedFunction::read(Parser &reader) {
    nameCpIndex = reader.readS4be();
    flags = reader.readS8be();
    typeCpIndex = reader.readS4be();
}

void TableFieldNameList::read(Parser &reader) {
    size = reader.readS4be();
    fieldNameCpIndex = std::vector<int32_t>();
    fieldNameCpIndex.reserve(size);
    for (auto i = 0; i < size; i++) {
        fieldNameCpIndex.push_back(reader.readS4be());
    }
}

void RecordField::read(Parser &reader) {
    nameCpIndex = reader.readS4be();
    flags = reader.readS8be();
    doc = std::make_unique<Markdown>();
    doc->read(reader);
    typeCpIndex = reader.readS4be();
}

StringCpInfo::StringCpInfo() { setTag(TAG_ENUM_CP_ENTRY_STRING); }

void StringCpInfo::read(Parser &reader) {
    auto stringLength = reader.readS4be();
    std::vector<char> result(stringLength);
    reader.readChars(result.data(), stringLength);
    value = std::string(result.data(), stringLength);
}

ShapeCpInfo::ShapeCpInfo() { setTag(TAG_ENUM_CP_ENTRY_SHAPE); }

void ShapeCpInfo::read(Parser &reader) {
    shapeLength = reader.readS4be();
    typeTag = TypeTag(reader.readU1());
    nameIndex = reader.readS4be();
    typeFlag = reader.readS8be();
    typeSpecialFlag = reader.readS4be();
    [[maybe_unused]] int32_t shapeTypeLengthInfo = shapeLength - 13;
    switch (typeTag) {
    case TYPE_TAG_INVOKABLE: {
        uint8_t isAnyFunction = reader.readU1();
        if (isAnyFunction == 0U) {
            paramCount = reader.readS4be();
            for (auto i = 0; i < paramCount; i++) {
                int32_t paramTypeCpIndex = reader.readS4be();
                addParam(paramTypeCpIndex);
            }
            hasRestType = reader.readU1();
            if (hasRestType != 0U) {
                restTypeIndex = reader.readS4be();
            }
            returnTypeIndex = reader.readS4be();
        }
        break;
    }
    case TYPE_TAG_MAP: {
        constraintTypeCpIndex = reader.readS4be();
        break;
    }
    case TYPE_TAG_ERROR: {
        [[maybe_unused]] int32_t pkgIdCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t errorTypeNameCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t detailTypeCpIndex = reader.readS4be();
        auto typeIds = std::make_unique<TypeId>();
        typeIds->read(reader);
        break;
    }
    case TYPE_TAG_STREAM: {
        constraintTypeCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t hasErrorType = reader.readU1();
        if (hasErrorType != 0) {
            [[maybe_unused]] int32_t errorTypeCpIndex = reader.readS4be();
        }
        break;
    }
    case TYPE_TAG_TYPEDESC: {
        constraintTypeCpIndex = reader.readS4be();
        break;
    }
    case TYPE_TAG_PARAMETERIZED_TYPE: {
        [[maybe_unused]] int32_t paramValueTypeCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t paramIndex = reader.readS4be();
        break;
    }
    case TYPE_TAG_FUTURE: {
        constraintTypeCpIndex = reader.readS4be();
        break;
    }
    case TYPE_TAG_OBJECT: {
        [[maybe_unused]] uint8_t isObjectType = reader.readU1();
        [[maybe_unused]] int32_t pkdIdCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t nameCpIndex = reader.readS4be();
        [[maybe_unused]] uint8_t isAbstract = reader.readU1();
        [[maybe_unused]] uint8_t isClient = reader.readU1();
        int32_t objectFieldsCount = reader.readS4be();
        auto objectFields = std::vector<std::unique_ptr<ObjectField>>();
        objectFields.reserve(objectFieldsCount);
        for (auto i = 0; i < objectFieldsCount; i++) {
            auto objectField = std::make_unique<ObjectField>();
            objectField->read(reader);
            objectFields.push_back(std::move(objectField));
        }
        uint8_t hasGeneratedInitFunction = reader.readU1();
        if (hasGeneratedInitFunction != 0U) {
            auto generatedInitFunction = std::make_unique<ObjectAttachedFunction>();
            generatedInitFunction->read(reader);
        }
        uint8_t hasInitFunction = reader.readU1();
        if (hasInitFunction != 0U) {
            auto initFunction = std::make_unique<ObjectAttachedFunction>();
            initFunction->read(reader);
        }
        int32_t objectAttachedFunctionsCount = reader.readS4be();
        auto objectAttachedFunctions = std::vector<std::unique_ptr<ObjectAttachedFunction>>();
        objectAttachedFunctions.reserve(objectAttachedFunctionsCount);
        for (auto i = 0; i < objectAttachedFunctionsCount; i++) {
            auto objectAttachedFunction = std::make_unique<ObjectAttachedFunction>();
            objectAttachedFunction->read(reader);
            objectAttachedFunctions.push_back(std::move(objectAttachedFunction));
        }
        int32_t typeInclusionsCount = reader.readS4be();
        auto typeInclusionsCpIndex = std::vector<int32_t>();
        typeInclusionsCpIndex.reserve(typeInclusionsCount);
        for (auto i = 0; i < typeInclusionsCount; i++) {
            typeInclusionsCpIndex.push_back(reader.readS4be());
        }
        auto typeIds = std::make_unique<TypeId>();
        typeIds->read(reader);
        break;
    }
    case TYPE_TAG_UNION: {
        [[maybe_unused]] uint8_t isCyclic = reader.readU1();
        uint8_t hasName = reader.readU1();
        if (hasName != 0U) {
            [[maybe_unused]] int32_t pkdIdCpIndex = reader.readS4be();
            [[maybe_unused]] int32_t nameCpIndex = reader.readS4be();
        }
        [[maybe_unused]] int32_t memberTypeCount = reader.readS4be();
        auto memberTypeCpIndex = std::vector<int32_t>();
        memberTypeCpIndex.reserve(memberTypeCount);
        for (auto i = 0; i < memberTypeCount; i++) {
            memberTypeCpIndex.push_back(reader.readS4be());
        }
        [[maybe_unused]] int32_t originalMemberTypeCount = reader.readS4be();
        auto originalMemberTypeCpIndex = std::vector<int32_t>();
        originalMemberTypeCpIndex.reserve(originalMemberTypeCount);
        for (auto i = 0; i < originalMemberTypeCount; i++) {
            originalMemberTypeCpIndex.push_back(reader.readS4be());
        }
        uint8_t isEnumType = reader.readU1();
        if (isEnumType != 0U) {
            [[maybe_unused]] int32_t pkgCpIndex = reader.readS4be();
            [[maybe_unused]] int32_t enumName = reader.readS4be();
            [[maybe_unused]] int32_t enumMemberSize = reader.readS4be();
            auto enumMembers = std::vector<int32_t>();
            enumMembers.reserve(enumMemberSize);
            for (auto i = 0; i < enumMemberSize; i++) {
                enumMembers.push_back(reader.readS4be());
            }
        }
        break;
    }
    case TYPE_TAG_TUPLE: {
        [[maybe_unused]] int32_t tupleTypesCount = reader.readS4be();
        auto tupleTypeCpIndex = std::vector<int32_t>();
        tupleTypeCpIndex.reserve(tupleTypesCount);
        for (auto i = 0; i < tupleTypesCount; i++) {
            tupleTypeCpIndex.push_back(reader.readS4be());
        }
        hasRestType = reader.readU1();
        if (hasRestType != 0U) {
            [[maybe_unused]] int32_t restTypeCpIndex = reader.readS4be();
        }
        break;
    }
    case TYPE_TAG_INTERSECTION: {
        [[maybe_unused]] int32_t constituentTypesCount = reader.readS4be();
        auto constituentTypeCpIndex = std::vector<int32_t>();
        constituentTypeCpIndex.reserve(constituentTypesCount);
        for (auto i = 0; i < constituentTypesCount; i++) {
            constituentTypeCpIndex.push_back(reader.readS4be());
        }
        [[maybe_unused]] int32_t effectiveTypeCount = reader.readS4be();
        break;
    }
    case TYPE_TAG_XML: {
        constraintTypeCpIndex = reader.readS4be();
        break;
    }
    case TYPE_TAG_TABLE: {
        constraintTypeCpIndex = reader.readS4be();
        uint8_t hasFieldNameList = reader.readU1();
        if (hasFieldNameList != 0U) {
            auto fieldNameList = std::make_unique<TableFieldNameList>();
            fieldNameList->read(reader);
        }
        uint8_t hasKeyConstraintType = reader.readU1();
        if (hasKeyConstraintType != 0U) {
            [[maybe_unused]] int32_t keyConstraintTypeCpIndex = reader.readS4be();
        }
        break;
    }
    case TYPE_TAG_RECORD: {
        [[maybe_unused]] int32_t pkdIdCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t nameCpIndex = reader.readS4be();
        [[maybe_unused]] uint8_t isSealed = reader.readU1();
        [[maybe_unused]] int32_t resetFieldTypeCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t recordFieldCount = reader.readS4be();
        auto recordFields = std::vector<std::unique_ptr<RecordField>>();
        recordFields.reserve(recordFieldCount);
        for (auto i = 0; i < recordFieldCount; i++) {
            auto recordField = std::make_unique<RecordField>();
            recordField->read(reader);
            recordFields.push_back(std::move(recordField));
        }
        [[maybe_unused]] int32_t hasInitFunction = reader.readU1();
        if (hasInitFunction != 0U) {
            auto recordInitFunction = std::make_unique<ObjectAttachedFunction>();
            recordInitFunction->read(reader);
        }
        [[maybe_unused]] int32_t typeInclusionsCount = reader.readS4be();
        auto typeInclusionsCpIndex = std::vector<int32_t>();
        typeInclusionsCpIndex.reserve(typeInclusionsCount);
        for (auto i = 0; i < typeInclusionsCount; i++) {
            typeInclusionsCpIndex.push_back(reader.readS4be());
        }
        break;
    }
    case TYPE_TAG_FINITE: {
        [[maybe_unused]] int32_t nameCpIndex = reader.readS4be();
        [[maybe_unused]] int64_t flags = reader.readS8be();
        int32_t valueSpaceSize = reader.readS4be();
        for (auto i = 0; i < valueSpaceSize; i++) {
            [[maybe_unused]] int32_t fTypeCpIndex = reader.readS4be();
            int32_t valueLength = reader.readS4be();
            for (auto j = 0; j < valueLength; j++) {
                [[maybe_unused]] int8_t tmp = reader.readU1();
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
        state = reader.readU1();
        size = reader.readS4be();
        elementTypeCpIndex = reader.readS4be();
        break;
    }
    default:
        std::cerr << "Invalid Type Tag in shape: " << typeTag << std::endl;
        abort();
        break;
    }
}

PackageCpInfo::PackageCpInfo() {
    orgIndex = 0;
    nameIndex = 0;
    versionIndex = 0;
    setTag(TAG_ENUM_CP_ENTRY_PACKAGE);
}

void PackageCpInfo::read(Parser &reader) {
    orgIndex = reader.readS4be();
    nameIndex = reader.readS4be();
    versionIndex = reader.readS4be();
}

IntCpInfo::IntCpInfo() { setTag(TAG_ENUM_CP_ENTRY_INTEGER); }

void IntCpInfo::read(Parser &reader) { value = reader.readS8be(); }

BooleanCpInfo::BooleanCpInfo() { setTag(TAG_ENUM_CP_ENTRY_BOOLEAN); }

void BooleanCpInfo::read(Parser &reader) { value = reader.readU1(); }

FloatCpInfo::FloatCpInfo() { setTag(TAG_ENUM_CP_ENTRY_FLOAT); }

void FloatCpInfo::read(Parser &reader) { value = reader.readS8bef(); }

ByteCpInfo::ByteCpInfo() { setTag(TAG_ENUM_CP_ENTRY_BYTE); }

void ByteCpInfo::read(Parser &reader) { value = reader.readU1(); }

void ConstantPoolSet::read(Parser &reader) {

    int constantPoolEntries = reader.readS4be();
    poolEntries = std::vector<std::unique_ptr<ConstantPoolEntry>>();
    poolEntries.reserve(constantPoolEntries);

    for (auto i = 0; i < constantPoolEntries; i++) {
        auto tag = static_cast<ConstantPoolEntry::tagEnum>(reader.readU1());
        switch (tag) {
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
            auto poolEntry = std::make_unique<PackageCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_SHAPE: {
            auto poolEntry = std::make_unique<ShapeCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_STRING: {
            auto poolEntry = std::make_unique<StringCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_INTEGER: {
            auto poolEntry = std::make_unique<IntCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_FLOAT: {
            auto poolEntry = std::make_unique<FloatCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BOOLEAN: {
            auto poolEntry = std::make_unique<BooleanCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_BYTE: {
            auto poolEntry = std::make_unique<ByteCpInfo>();
            poolEntry->read(reader);
            poolEntries.push_back(std::move(poolEntry));
            break;
        }
        default:
            break;
        }
    }
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

} // namespace nballerina
