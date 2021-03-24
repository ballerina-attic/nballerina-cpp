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

#ifndef __STRUCTUREINSN__H__
#define __STRUCTUREINSN__H__

#include "NonTerminatorInsn.h"
#include <vector>

namespace nballerina {

// Forward Declaration
class Operand;
class Variable;
class MapConstruct;

class StructureInsn : public NonTerminatorInsn {
  private:
    std::vector<std::unique_ptr<MapConstruct>> initValues;
    void mapInstructionTranslate(const Variable &lhsVar, LLVMModuleRef &modRef);
    LLVMValueRef getNewMapIntDeclaration(LLVMModuleRef &modRef);

  public:
    StructureInsn() = delete;
    StructureInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB);
    StructureInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB,
                  std::vector<std::unique_ptr<MapConstruct>> initValues);
    ~StructureInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__STRUCTUREINSN__H__
