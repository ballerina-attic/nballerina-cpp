# This repository contains code for translating Ballerina IR to LLVM IR

## Prerequisites for Ubuntu 20.04
* `sudo apt install build-essential llvm-11-dev cmake cargo python3-pip`
* `cargo install cbindgen`
* `pip3 install lit filecheck`

## Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Debug`
3. `make -j`
This will build the Rust runtime (output in runtime/target/release), the C++ header for the runtime (output in runtime/include/) and the nballerinacc app.

## Usage
* Output binary will be at `build/nballerinacc`
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc ../BIR2llvmIR/main-bir-dump
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
 
        clang++-11 -O0 main-bir-dump.ll
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. 
* The a.out can be disassembled using "objdump -d" to see the machine instructions generated in main.
* Optional: Run `export LD_LIBRARY_PATH=<Full path to runtime .so>` during development so that the dynamic loader can find the nBallerina runtime libs at app runtime.
* Execute the tests in `test` folder:

        cd build/
        make check
* To add LIT tests, add LIT compatible .bal files to the `test/` folder
