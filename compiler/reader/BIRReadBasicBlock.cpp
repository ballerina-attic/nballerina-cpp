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

#include "reader/BIRReadBasicBlock.h"
#include "reader/BIRReadInsns.h"

namespace nballerina {

void BIRReadBasicBlock::readBasicBlock(Function &birFunction, Parser &reader, ConstantPoolSet &cp, bool ignore) {
    int32_t nameCpIndex = reader.readS4be();
    birFunction.basicBlocks.emplace_back(cp.getStringCp(nameCpIndex), &birFunction);
    auto &basicBlock = birFunction.basicBlocks.back();

    int32_t insnCount = reader.readS4be();
    basicBlock.instructions.reserve(insnCount);
    for (auto i = 0; i < insnCount; i++) {
        BIRReadInsn::readInsn(basicBlock, reader, cp);
    }
    if (ignore) {
        birFunction.basicBlocks.pop_back();
    }
}

} // namespace nballerina
