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
#include "Package.h"
#include <fstream>
#include <iostream>
#include <llvm-c/Core.h>
#include <llvm/ADT/Triple.h>
#include <memory>
#include <string>
#include <vector>

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

    char *message;
    bool dumpLlvm = true; // temp value
    string moduleName = birPackage->getOrgName() + birPackage->getPackageName() + birPackage->getVersion();
    LLVMModuleRef mod = LLVMModuleCreateWithName(moduleName.c_str());
    const char *tripleStr = LLVM_DEFAULT_TARGET_TRIPLE;

    // MacOS specific code. This is needed, since the default Triple will have the
    // OS as darwin, but the clang will expect the os as macosx
    llvm::Triple triple(LLVM_DEFAULT_TARGET_TRIPLE);
    char modifiedTriple[200];
    if (triple.isMacOSX()) {
        unsigned major, minor, micro;
        if (triple.getMacOSXVersion(major, minor, micro)) {
            triple.setOS(llvm::Triple::OSType::MacOSX);
            sprintf(modifiedTriple, "%s%i.%i.%i", triple.getTriple().c_str(), major, minor, micro);
            tripleStr = modifiedTriple;
        }
    }

    LLVMSetSourceFileName(mod, birPackage->getSrcFileName().c_str(), birPackage->getSrcFileName().length());
    LLVMSetDataLayout(mod, "e-m:e-i64:64-f80:128-n8:16:32:64-S128");
    LLVMSetTarget(mod, tripleStr);
    birPackage->translate(mod);

    if (dumpLlvm) {
        if (LLVMPrintModuleToFile(mod, outFileName.c_str(), &message)) {
            std::cerr << message;
        }
    }
}
