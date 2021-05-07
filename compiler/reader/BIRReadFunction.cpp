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

#include "reader/BIRReadFunction.h"
#include "bir/Package.h"
#include "reader/BIRReadBasicBlock.h"
#include <queue>
#include <string>
#include <vector>

namespace nballerina {

static bool ignoreFunction(const std::string &funcName) {
    const std::vector<std::string> ignoreNames{".<init>", ".<start>", ".<stop>", "..<init>", "..<start>", "..<stop>"};
    bool ignoreFunction = false;
    for (const auto &name : ignoreNames) {
        if (funcName.rfind(name, 0) == 0) {
            ignoreFunction = true;
            break;
        }
    }
    return ignoreFunction;
}

void BIRReadFunction::readLocalVar(Function &function, Parser &reader, ConstantPoolSet &cp) {
    uint8_t kind = reader.readU1();
    int32_t typeCpIndex = reader.readS4be();
    auto type = cp.getTypeCp(typeCpIndex, false);
    int32_t nameCpIndex = reader.readS4be();

    if (kind == ARG_VAR_KIND) {
        [[maybe_unused]] int32_t metaVarNameCpIndex = reader.readS4be();
    } else if (kind == LOCAL_VAR_KIND) {
        // enclosing basic block id
        [[maybe_unused]] int32_t metaVarNameCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t endBbIdCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t startBbIdCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t instructionOffset = reader.readS4be();
    }
    function.localVars.emplace_back(std::move(type), cp.getStringCp(nameCpIndex), (VarKind)kind);
}

void BIRReadFunction::readFunction(Package &package, Parser &reader, ConstantPoolSet &cp, bool ignore) {

    // Read debug info
    // position
    int32_t sourceFileCpIndex = reader.readS4be();
    int32_t sLine = reader.readS4be();
    int32_t sCol = reader.readS4be();
    int32_t eLine = reader.readS4be();
    int32_t eCol = reader.readS4be();
    Location location(cp.getStringCp(sourceFileCpIndex), (int)sLine, (int)eLine, (int)sCol, (int)eCol);

    // TODO should not set src for every function
    package.sourceFileName = cp.getStringCp(sourceFileCpIndex);

    int32_t nameCpIndex = reader.readS4be();
    std::string functionName = cp.getStringCp(nameCpIndex);
    int32_t workdernameCpIndex = reader.readS4be();
    int32_t flags = reader.readS8be();
    [[maybe_unused]] uint8_t origin = reader.readU1();
    int32_t typeCpIndex = reader.readS4be();
    [[maybe_unused]] auto invocable_type = cp.getInvocableType(typeCpIndex);

    package.functions.emplace_back(&package, functionName, cp.getStringCp(workdernameCpIndex), flags);
    auto &birFunction = package.functions.back();
    birFunction.setLocation(location);

    // annotation_attachments_content
    int64_t annotationLength = reader.readS8be(); // annotation_attachments_content_length
    reader.ignore(annotationLength);

    int32_t requiredParamCount = reader.readS4be();

    // Set function param here and then fill remaining values from the default Params
    std::queue<Operand> functionParams;
    for (auto i = 0; i < requiredParamCount; i++) {
        int32_t paramNameCpIndex = reader.readS4be();
        functionParams.emplace(Operand(cp.getStringCp(paramNameCpIndex), ARG_VAR_KIND));
        [[maybe_unused]] int64_t paramFlags = reader.readS8be();
    }

    uint8_t hasRestParam = reader.readU1();
    [[maybe_unused]] int32_t restParamNameCpIndex = 0;
    if (hasRestParam != 0U) {
        restParamNameCpIndex = reader.readS4be();
    }

    [[maybe_unused]] uint8_t hasReceiver = reader.readU1();
    if (hasReceiver != 0U) {
        [[maybe_unused]] uint8_t receiverKind = reader.readU1();
        [[maybe_unused]] int32_t receiverTypeCpIndex = reader.readS4be();
        [[maybe_unused]] int32_t receiverNameCpIndex = reader.readS4be();
    }

    auto taintTableLength = reader.readS8be();
    reader.ignore(taintTableLength);

    auto docLength = reader.readS4be();
    reader.ignore(docLength);

    int32_t depended_global_var_length = reader.readS4be();
    for (auto i = 0; i < depended_global_var_length; i++) {
        [[maybe_unused]] int32_t depended_global_var_cp_entry = reader.readS4be();
    }
    [[maybe_unused]] int64_t scopeTableLength = reader.readS8be();
    int32_t scopeEntryCount = reader.readS4be();
    // scope entry
    for (auto i = 0; i < scopeEntryCount; i++) {
        [[maybe_unused]] int32_t currentScopeIndex = reader.readS4be();
        [[maybe_unused]] int32_t instructionOffset = reader.readS4be();
        uint8_t hasParent = reader.readU1();
        if (hasParent != 0U) {
            [[maybe_unused]] int32_t parentScopeIndex = reader.readS4be();
        }
    }

    [[maybe_unused]] int64_t functionBodyLength = reader.readS8be();
    // function body
    [[maybe_unused]] int32_t argsCount = reader.readS4be();
    uint8_t hasReturnVar = reader.readU1();

    // returnVar
    if (hasReturnVar != 0U) {
        uint8_t kind = reader.readU1();
        int32_t typeCpIndex = reader.readS4be();
        auto type = cp.getTypeCp(typeCpIndex, false);
        int32_t nameCpIndex = reader.readS4be();
        birFunction.returnVar = Variable(std::move(type), cp.getStringCp(nameCpIndex), (VarKind)kind);
    }

    int32_t paramsWithDefaults = reader.readS4be();
    birFunction.requiredParams.reserve(paramsWithDefaults);
    for (auto i = 0; i < paramsWithDefaults; i++) {
        uint8_t kind = reader.readU1();
        int32_t typeCpIndex = reader.readS4be();
        birFunction.requiredParams.emplace_back(std::move(functionParams.front()), cp.getTypeCp(typeCpIndex, false));
        functionParams.pop();
        [[maybe_unused]] int32_t nameCpIndex = reader.readS4be();
        if (kind == ARG_VAR_KIND) {
            [[maybe_unused]] int32_t metaVarNameCpIndex = reader.readS4be();
        }
        [[maybe_unused]] uint8_t hasDefaultExpr = reader.readU1();
    }

    int32_t localVarCount = reader.readS4be();
    birFunction.localVars.reserve(localVarCount);
    for (auto i = 0; i < localVarCount; i++) {
        readLocalVar(birFunction, reader, cp);
    }

    for (auto i = 0; i < paramsWithDefaults; i++) {
        // default parameter basic blocks info
        int32_t defaultParameterBBCount = reader.readS4be();
        for (auto i = 0; i < defaultParameterBBCount; i++) {
            BIRReadBasicBlock::readBasicBlock(birFunction, reader, cp, true);
        }
    }

    // basic block info
    int32_t BBCount = reader.readS4be();
    birFunction.basicBlocks.reserve(BBCount);
    for (auto i = 0; i < BBCount; i++) {
        BIRReadBasicBlock::readBasicBlock(birFunction, reader, cp);
    }

    // error table
    [[maybe_unused]] int32_t errorEntriesCount = reader.readS4be();
    [[maybe_unused]] int32_t channelsLength = reader.readS4be();

    if (ignore || ignoreFunction(functionName)) {
        package.functions.pop_back();
    }
}
} // namespace nballerina
