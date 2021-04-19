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

#include "BIRReader.h"
#include "CodeGenerator.h"
#include "Package.h"
#include <iostream>
#include <string>

using namespace std;
using namespace nballerina;

BIRReader BIRReader::reader;

int main(int argc, char **argv) {
    string inFileName = "";
    string outFileName = "";
    string exeName = "";
    if (argc <= 1) {
        printf("Need input file name \n");
        exit(0);
    }

    int i = 0;
    while (i < argc) {
        string arg = std::string(argv[i]);
        if (arg == "-c") {
            i = i + 1;
        } else if (arg == "-o") {
            outFileName = argv[i + 1];
            i = i + 2;
        } else {
            inFileName = argv[i];
            i = i + 1;
        }
    }
    // if output file name is empty from command line options.
    if (outFileName == "") {
        for (unsigned int i = 0; i < inFileName.length(); i++) {
            char tmpstr = inFileName[i];
            if (tmpstr == '.')
                break;
            outFileName = outFileName + inFileName[i];
        }
        outFileName = outFileName + ".ll";
    }

    BIRReader::reader.setFileStream(inFileName);

    std::shared_ptr<nballerina::Package> birPackage = BIRReader::reader.deserialize();

    // Codegen
    return CodeGenerator::generateLLVMIR(birPackage.get(), outFileName, birPackage->getModuleName());
}
