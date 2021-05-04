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

#include "codegen/CodeGenerator.h"
#include "bir/Package.h"
#include "codegen/PackageCodeGen.h"
#include <llvm/ADT/Triple.h>
#include <llvm/IR/LLVMContext.h>
#include <iostream>
#include <sstream>

namespace nballerina {

int CodeGenerator::generateLLVMIR(Package &translatableObj, const std::string &outFileName) {

    auto mContext = llvm::LLVMContext();
    auto mod = llvm::Module(translatableObj.getModuleName(), mContext);
    auto builder = llvm::IRBuilder<>(mContext);

    // MacOS specific code. This is needed, since the default Triple will have the
    // OS as darwin, but the clang will expect the os as macosx
    llvm::Triple triple(LLVM_DEFAULT_TARGET_TRIPLE);
    std::string tripleString = triple.getTriple();
    if (triple.isMacOSX()) {
        unsigned int major = 0;
        unsigned int minor = 0;
        unsigned int micro = 0;
        if (triple.getMacOSXVersion(major, minor, micro)) {
            triple.setOS(llvm::Triple::OSType::MacOSX);
            std::stringstream tripleStrStream(triple.getTriple());
            tripleStrStream << major << "." << minor << "." << micro;
            tripleString = tripleStrStream.str();
        }
    }

    mod.setDataLayout("e-m:e-i64:64-f80:128-n8:16:32:64-S128");
    mod.setTargetTriple(tripleString);

    // Codegen
    PackageCodeGen generator(mod);
    generator.visit(translatableObj, builder);

    // Write LLVM IR to file
    std::error_code EC;
    auto outStream = llvm::raw_fd_ostream(outFileName, EC);
    if (EC) {
        std::cerr << EC.message();
        return -1;
    }
    mod.print(outStream, nullptr);
    return 0;
}

} // namespace nballerina
