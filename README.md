# nBallerina
Translate Ballerina IR to LLVM IR.

## Build from source 
### Prerequisites : Ubuntu 20.04
* `sudo apt install build-essential llvm-11-dev cmake cargo python3-pip`
* `cargo install cbindgen`
* `pip3 install lit filecheck`

### Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Debug`
3. `make -j`

This will build:
* The Rust runtime : runtime/target/release/libnballerinart.so
* The C++ header for the runtime : runtime/include/nballerina_rt.h
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Run tests
1. `cd build/`
2. `make check` 

## Usage
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc ../BIR2llvmIR/main-bir-dump
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
 
        clang -O0 main-bir-dump.ll
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. 
* The a.out can be disassembled using "objdump -d" to see the machine instructions
