# This repository contains code for translating Ballerina IR to LLVM IR

## Prerequisites for Ubuntu 20.04
* `sudo apt install build-essential llvm-11-dev cmake`

## Build steps
1.  `cd build`
2. `cmake ../`
3. `make`

## Usage
* Output binary will be at `build/nballerinacc`
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc ../BIR2llvmIR/main-bir-dump
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
 
        clang++-11 -O0 main-bir-dump.ll
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. 
* The a.out can be disassembled using "objdump -d" to see the machine instructions generated in main.

### Run integration tests (LIT tests)
* Install standalone Pyhton LIT package `pip3 install lit filecheck`
* Execute tests:

        cd build/
        make check
* To add LIT tests, add LIT compatible .bal files to the `test/` folder
