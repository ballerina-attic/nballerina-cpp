# nBallerina
Translate Ballerina IR to LLVM IR.

## Building from source in Ubuntu 20.04
### Prerequisites
* `sudo apt install build-essential llvm-11-dev cmake cargo python3-pip`
* `cargo install cbindgen`
* `pip3 install lit filecheck`

### Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Debug`
3. `make -j`

This will build:
* The Rust runtime : runtime/target/release/libballerina_rt.so
* The C++ header for the runtime : runtime/include/ballerina_rt.h
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Run tests
1. Install Java 8 runtime (e.g. openjdk-8-jre on ubuntu)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [link](https://drive.google.com/file/d/1a1TlJdw-rTtCLOFrrvJe4nr1FkGzwpKH/view?usp=sharing)
4. Extract Ballerina pack and add the `bin` folder with the `ballerina` executable to your `PATH` system variable 
5. Navigate to nBallerina `build/` folder and run tests

        cd build/
        make check


## Building from source in MacOSX 10.15.0
### Prerequisites
* If you haven’t installed xcode and brew yet, please install them. Following steps assumes that xcode and brew are already installed. 
* `brew install llvm@11 rust python3`
* `brew postinstall python3`
* `cargo install cbindgen`
* cargo installs the dependencies in user's home directory. Hence, add `~/.cargo/bin` to your `PATH` system variable. `export PATH=$PATH:~/.cargo/bin`
* `pip3 install lit filecheck`

### Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm/ -DCMAKE_CXX_STANDARD=14`
3. `make -j`

This will build:
* The Rust runtime : runtime/target/release/libballerina_rt.so
* The C++ header for the runtime : runtime/include/ballerina_rt.h
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Run tests
1. Install Java 8 runtime (e.g. openjdk-8-jre on ubuntu)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [link](https://drive.google.com/file/d/1a1TlJdw-rTtCLOFrrvJe4nr1FkGzwpKH/view?usp=sharing)
4. Extract Ballerina pack and add the `bin` folder with the `ballerina` executable to your `PATH` system variable 
5. Navigate to nBallerina `build/` folder and run tests

        cd build/
        make check


## Usage
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc ../BIR2llvmIR/main-bir-dump
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
 
        clang -O0 main-bir-dump.ll
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. 
* The a.out can be disassembled using "objdump -d" to see the machine instructions
