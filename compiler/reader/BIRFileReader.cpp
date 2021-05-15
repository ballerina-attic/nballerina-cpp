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

#include "reader/BIRFileReader.h"
#include "reader/BIRReadPackage.h"
#include "reader/ConstantPool.h"

namespace nballerina {

BIRFileReader::BIRFileReader(const std::string &FileName) { is.open(FileName, std::ifstream::binary); }

std::shared_ptr<Package> BIRFileReader::deserialize(const std::string &FileName) {
    // Open File
    BIRFileReader reader{FileName};
    // Read Constant Pool
    ConstantPoolSet cp;
    cp.read(reader);
    // Read Module
    return BIRReadPackage::readModule(reader, cp);
}

static bool isLittleEndian() {
    unsigned int val = 1;
    auto *c = reinterpret_cast<char *>(&val);
    return (int)*c != 0;
}

template <typename T>
T swapEndian(T u) {
    static_assert(CHAR_BIT == 8);
    union {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++) {
        dest.u8[k] = source.u8[sizeof(T) - k - 1];
    }
    return dest.u;
}

// Read 1 byte from the stream
uint8_t BIRFileReader::readU1() {
    uint8_t value = 0;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

// Read 2 bytes from the stream
int16_t BIRFileReader::readS2be() {
    int16_t value = 0;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<int16_t>(value);
}

// Read 4 bytes from the stream
int32_t BIRFileReader::readS4be() {
    int32_t value = 0;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<int32_t>(value);
}

// Read 8 bytes from the stream
int64_t BIRFileReader::readS8be() {
    int64_t value = 0;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<int64_t>(value);
}

// Read 8 bytes from the stream for float value
double BIRFileReader::readS8bef() {
    double value = NAN;
    char *p = reinterpret_cast<char *>(&value);
    is.read(p, sizeof(value));
    if (!isLittleEndian()) {
        return value;
    }
    return swapEndian<double>(value);
}

void BIRFileReader::ignore(std::streamsize length) { is.ignore(length); }

void BIRFileReader::readChars(char *outBuff, std::streamsize length) { is.read(outBuff, length); }

} // namespace nballerina
