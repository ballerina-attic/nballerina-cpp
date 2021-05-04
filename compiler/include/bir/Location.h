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

#ifndef __LOCATION__H__
#define __LOCATION__H__

#include <string>

namespace nballerina {

class Location {
  private:
    std::string fileName;
    int sLine;
    int sCol;
    int eLine;
    int eCol;

  public:
    Location() = default;
    Location(std::string name, int sline, int scol, int eline, int ecol);
    ~Location() = default;

    const std::string &getFileName() const;
    int getStartLineNum() const;
    int getStartColumnNum() const;
    int getEndLineNum() const;
    int getEndColumnNum() const;
};

} // namespace nballerina

#endif //!__LOCATION__H__
