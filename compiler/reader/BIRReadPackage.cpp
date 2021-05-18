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

#include "reader/BIRReadPackage.h"
#include "reader/BIRReadFunction.h"

namespace nballerina {

// Read Global Variable and push it to BIRPackage
void BIRReadPackage::readGlobalVar(Package &birPackage, Parser &reader, ConstantPoolSet &cp) {
    uint8_t kind = reader.readU1();

    int32_t varDclNameCpIndex = reader.readS4be();

    [[maybe_unused]] int64_t flags = reader.readS8be();
    [[maybe_unused]] uint8_t origin = reader.readU1();
    // Markdown
    int32_t docLength = reader.readS4be();
    reader.ignore(docLength);

    int32_t typeCpIndex = reader.readS4be();
    auto type = cp.getTypeCp(typeCpIndex, false);
    birPackage.globalVars.emplace_back(std::move(type), cp.getStringCp(varDclNameCpIndex), (VarKind)kind);
}

std::shared_ptr<Package> BIRReadPackage::readModule(Parser &reader, ConstantPoolSet &cp) {

    int32_t idCpIndex = reader.readS4be();
    ConstantPoolEntry *poolEntry = cp.getEntry(idCpIndex);
    auto birPackage = std::make_shared<Package>();

    switch (poolEntry->getTag()) {
    case ConstantPoolEntry::tagEnum::TAG_ENUM_CP_ENTRY_PACKAGE: {
        auto *packageEntry = static_cast<PackageCpInfo *>(poolEntry);
        poolEntry = cp.getEntry(packageEntry->getOrgIndex());
        auto *stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->org = stringCp->getValue();

        poolEntry = cp.getEntry(packageEntry->getNameIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->name = stringCp->getValue();

        poolEntry = cp.getEntry(packageEntry->getVersionIndex());
        stringCp = static_cast<StringCpInfo *>(poolEntry);
        birPackage->version = stringCp->getValue();
        break;
    }
    default:
        break;
    }

    // The following three are read into unused variables so that the file
    // pointer advances to the data that we need next.
    int32_t importCount = 0;
    int32_t constCount = 0;
    int32_t typeDefinitionCount = 0;

    importCount = reader.readS4be();
    int32_t importSize = importCount * 12; // package_cp_info is 12 bytes
    reader.ignore(importSize);

    constCount = reader.readS4be();
    for (auto i = 0; i < constCount; i++) {
        [[maybe_unused]] int32_t constNameCpIndex = reader.readS4be();
        [[maybe_unused]] int64_t constFlags = reader.readS8be();
        [[maybe_unused]] uint8_t constOrigin = reader.readU1();
        reader.ignore(20);
        int32_t markdownLength = reader.readS4be();
        reader.ignore(markdownLength);
        [[maybe_unused]] int32_t constTypeCpIndex = reader.readS4be();
        int64_t constValueLength = reader.readS8be();
        reader.ignore(constValueLength);
    }

    typeDefinitionCount = reader.readS4be();
    for (auto i = 0; i < typeDefinitionCount; i++) {
        reader.ignore(20);
        [[maybe_unused]] int32_t tdNameCpIndex = reader.readS4be();
        [[maybe_unused]] int64_t tdFlags = reader.readS8be();
        [[maybe_unused]] uint8_t tdLabel = reader.readU1();
        [[maybe_unused]] uint8_t tdOrigin = reader.readU1();
        int32_t markdownLength = reader.readS4be();
        reader.ignore(markdownLength);
        int64_t annotationAttachmentsContentLength = reader.readS8be();
        reader.ignore(annotationAttachmentsContentLength);
        [[maybe_unused]] int32_t tdTypeCpIndex = reader.readS4be();
    }

    int32_t globalVarCount = reader.readS4be();
    birPackage->globalVars.reserve(globalVarCount);
    for (auto i = 0; i < globalVarCount; i++) {
        readGlobalVar(*birPackage, reader, cp);
    }

    int32_t typeDefinitionBodiesCount = reader.readS4be();
    for (auto i = 0; i < typeDefinitionBodiesCount; i++) {
        int32_t attachedFunctionsCount = reader.readS4be();
        for (auto j = 0; j < attachedFunctionsCount; j++) {
            BIRReadFunction::readFunction(*birPackage, reader, cp, true);
        }
        int32_t referencedTypesCount = reader.readS4be();
        for (auto j = 0; j < referencedTypesCount; j++) {
            [[maybe_unused]] int32_t referencedType = reader.readS4be();
        }
    }

    int32_t functionCount = reader.readS4be();
    birPackage->functions.reserve(functionCount);
    for (auto i = 0; i < functionCount; i++) {
        BIRReadFunction::readFunction(*birPackage, reader, cp);
    }

    return birPackage;
}

} // namespace nballerina
