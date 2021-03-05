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

#ifndef __BALCONSTANTLOAD__H__
#define __BALCONSTANTLOAD__H__

#include "BalNonTerminatorInsn.h"
#include "BalType.h"
#include <string>

namespace nballerina {

// TODO Convert to template class
class ConstantLoadInsn : public NonTerminatorInsn {
  private:
    TypeTag typeTag;
    union value {
        int intValue;
        double floatValue;
        bool boolValue;
        std::string *strValue;
        value() {}
        value(int x) : intValue(x) {}
        value(float x) : floatValue(x) {}
        value(bool x) : boolValue(x) {}
        value(std::string *x) : strValue(x) {}
    } val;

  public:
    ConstantLoadInsn() = delete;
    ConstantLoadInsn(Operand *lOp, BasicBlock *currentBB);
    ~ConstantLoadInsn() = default;

    int getIntValue();
    float getFloatValue();
    bool getBoolValue();
    std::string *getStringValue();
    TypeTag getTypeTag();

    void setIntValue(int intVal, TypeTag TypeTag);
    void setFloatValue(float floatVal, TypeTag TypeTag);
    void setBoolValue(bool boolVal, TypeTag TypeTag);
    void setStringValue(std::string *str, TypeTag TypeTag);
    void setTypeTagNil(TypeTag TypeTag);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONSTANTLOAD__H__
