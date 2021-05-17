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

#ifndef __BIRREADERUTILS__H__
#define __BIRREADERUTILS__H__

#include "bir/BasicBlock.h"
#include "interfaces/Parser.h"
#include "reader/ConstantPool.h"

namespace nballerina {

class BIRReadInsn {
  private:
    static void ReadCondBrInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadFuncCallInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadGoToInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadReturnInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadBinaryInsn(BasicBlock &currentBB, InstructionKind kind, Parser &reader, ConstantPoolSet &cp);
    static void ReadUnaryInsn(BasicBlock &currentBB, InstructionKind kind, Parser &reader, ConstantPoolSet &cp);
    static void ReadConstLoadInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadMoveInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadTypeDescInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadStructureInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadTypeCastInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadTypeTestInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadArrayInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadArrayStoreInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadArrayLoadInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadMapStoreInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);
    static void ReadMapLoadInsn(BasicBlock &currentBB, Parser &reader, ConstantPoolSet &cp);

  public:
    static void readInsn(BasicBlock &basicBlock, Parser &reader, ConstantPoolSet &cp);
};

} // namespace nballerina

#endif //!__BIRREADERUTILS__H__
