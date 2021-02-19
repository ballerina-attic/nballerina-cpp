# nBallerina
Translate Ballerina IR to LLVM IR.

## Building from source in Ubuntu 20.04
### Prerequisites
* `sudo apt install build-essential llvm-11-dev cmake cargo python3-pip`
* `pip3 install lit filecheck`

### Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Debug`
3. `make -j`

This will build:
* The Rust runtime : runtime/target/release/libballerina_rt.so
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Run tests
1. Install Java 8 runtime (e.g. openjdk-8-jre on ubuntu)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [link](https://drive.google.com/file/d/1a1TlJdw-rTtCLOFrrvJe4nr1FkGzwpKH/view?usp=sharing)
4. Extract Ballerina pack and add the `bin` folder with the `ballerina` executable to your `PATH` system variable 
5. Navigate to nBallerina `build/` folder and run tests

        cd build/
        make check

### Usage
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc ../compiler/main-bir-dump
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
 
        clang -O0 main-bir-dump.ll
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. 
* The a.out can be disassembled using "objdump -d" to see the machine instructions

### To codegen the C header for the Rust runtime lib
1. Install cbindgen

        cargo install cbindgen
2. Execute build command

        cd build/
        make runtime_header
3. The generated header will be at : runtime/include/ballerina_rt.h


## Building from source in MacOSX 10.15.0
### Prerequisites
* If you haven’t installed xcode and brew yet, please install them. Following steps assumes that xcode and brew are already installed. 
* `brew install llvm@11 rust python3`
* `brew postinstall python3`
* `pip3 install lit filecheck`

### Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm/ -DCMAKE_CXX_STANDARD=14`
3. `make -j`

This will build:
* The Rust runtime : runtime/target/release/libballerina_rt.so
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Run tests
1. Install Java 8 runtime (e.g. JRE 8 package for MacOS)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [link](https://drive.google.com/file/d/1a1TlJdw-rTtCLOFrrvJe4nr1FkGzwpKH/view?usp=sharing)
4. Extract Ballerina pack and add the `bin` folder with the `ballerina` executable to your `PATH` system variable 
5. Navigate to nBallerina `build/` folder and run tests

        cd build/
        make check

### Usage
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc ../compiler/main-bir-dump -o main.ll
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
 
        clang -O0 main.ll
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using `echo $?` will yield the int value returned by the function. 
* The a.out can be disassembled using `objdump -d` to see the machine instructions

### To codegen the C header for the Rust runtime lib
1. Install cbindgen

        cargo install cbindgen
  * cargo installs the dependencies in user's home directory. Hence, add `~/.cargo/bin` to your `PATH` system variable. `export PATH=$PATH:~/.cargo/bin`
2. Execute build command

        cd build/
        make runtime_header
3. The generated header will be at : runtime/include/ballerina_rt.h

## Building from source in Windows 10(x64)

### Prerequisites
* Install [cargo](https://doc.rust-lang.org/cargo/getting-started/installation.html)
* Install [python-3](https://www.python.org/downloads/)
* Build LLVM from source
  * Install [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019). Please make sure that tools should be installed with **C++ CMake tools for Windows** and **C++ ATL for latest v142 build tools (x86 & x64)** which are optional.
  * Download the LLVM source from [here](https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.1/llvm-11.0.1.src.tar.xz) and extract. 
  * Create the `build` directory. Run following commands in **x64 Native Tools Command Prompt for VS 2019**.
    
        cd build
        cmake -DCMAKE_BUILD_TYPE="Release" -Thost=x64 -G "Visual Studio 16 2019" -A x64 -DLLVM_USE_CRT_RELEASE=MT -DCMAKE_INSTALL_PREFIX="<LLVM_INSTALL_PATH>" ../ 
        msbuild INSTALL.vcxproj /p:configuration=release

   
### Build steps
Clone the nballerina source and run below commands.
1. `cd build`
2. `cmake -DLLVM_DIR=<LLVM_INSTALL_PATH>\lib\cmake\llvm ../`
3. `msbuild ALL_BUILD.vcxproj /p:configuration=Release`

### Usage
* Output binary will be at `Release\nballerinacc.exe`
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        nballerinacc.exe  ../../compiler/main-bir-dump -o main.ll

### Run tests
* Tests are currently not supported in Windows