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

#ifndef BIRREADER_H
#define BIRREADER_H

#include "bir/Package.h"
#include "interfaces/Parser.h"
#include <fstream>
#include <memory>

namespace nballerina {

class BIRFileReader : public Parser {
  private:
    std::ifstream is;
    BIRFileReader(std::string FileName);

    uint8_t readU1() override;
    int16_t readS2be() override;
    int32_t readS4be() override;
    int64_t readS8be() override;
    double readS8bef() override;
    void ignore(std::streamsize length) override;
    void readChars(char *outBuff, std::streamsize length) override;

  public:
    static std::shared_ptr<Package> deserialize(const std::string &FileName);
};

} // namespace nballerina

#endif // BIRREADER_H
