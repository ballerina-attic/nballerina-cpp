# This repository contains code for translating Ballerina IR to LLVM IR

 First build the LLVM and then build the nBallerina compiler.  Here are the steps:

[1] Checkout the sources and go to the build/llvm directory
[2] Issue the following command to configure the sources:
    cmake -DCMAKE_BUILD_TYPE="Release" \
      -DCMAKE_INSTALL_PREFIX="$LLVM_INSTALL_ROOT" \
      -DBUILD_SHARED_LIBS=True \
      -DLLVM_USE_SPLIT_DWARF=True \
      -DLLVM_OPTIMIZED_TABLEGEN=True \
      -DLLVM_BUILD_TESTS=True \
    ../../llvm-project-master/llvm

[3] Issue a make.

[4] Now change directory to ../../BIR2llvmIR and issue a make.  This will build nballerinacc

[5] Invoking nballerinacc will generate a foo.ll which is the LLVM output, in the following way -

./nballerinacc foo

where foo is bir-dump

[6] Now the foo.ll can be compiled into an a.out by invoking clang with -O0 option.  The -O0 is required because the optimizer will otherwise optimize everything away.  Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. The a.out can be disassembled using "objdump -d" to see the machine instructions generated in main.
