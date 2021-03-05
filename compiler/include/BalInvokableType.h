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

#ifndef __BALINVOKABLETYPE__H__
#define __BALINVOKABLETYPE__H__

#include <vector>

namespace nballerina {

// Forward Declaration
class Type;

class InvokableType {
  private:
    std::vector<Type *> paramTypes;
    Type *restType;
    Type *returnType;

  public:
    InvokableType() = delete;
    InvokableType(std::vector<Type *> paramTy, Type *restTy, Type *retTy);
    InvokableType(std::vector<Type *> paramTy, Type *retTy);
    ~InvokableType() = default;

    const Type *getReturnType();
    const Type *getRestType();
    Type *getParamType(int i);
    size_t getParamTypeCount();
};

} // namespace nballerina

#endif //!__BALINVOKABLETYPE__H__
