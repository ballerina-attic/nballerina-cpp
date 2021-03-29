# nBallerina
Translate Ballerina IR to LLVM IR.

## Building from source in Ubuntu 20.04
### Prerequisites
* `sudo apt install build-essential llvm-11-dev cmake cargo python3-pip`
* `pip3 install lit filecheck`

### Build steps
1. `cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/`
2. `cmake --build ./build/ -- -j`

This will build:
* The Rust runtime : runtime/target/release/libballerina_rt.so
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Usage
* Run nballerinacc against a BIR dump file (using `ballerina build -dump-bir-file=<output> <input>`) to generate the .ll LLVM IR file
 
        ./nballerinacc <bir dump file path>
* The .ll file can be compiled into an executable using clang and the compiled runtime library
 
        clang -O0 -o $filename.out $filename-bir-dump.ll -L../runtime/target/release/ -lballerina_rt

### Run tests
1. Install Java 8 runtime (e.g. openjdk-8-jre on ubuntu)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [jballerina-tools-2.0.0-Preview2-nballerina-r2.zip](https://github.com/ballerina-platform/ballerina-lang/packages/413010?version=2.0.0-Preview2-nballerina-r2)
4. Extract Ballerina pack and add the `bin` folder with the `ballerina` executable to your `PATH` system variable 
5. Run:

        cmake --build ./build/ -t check

### To codegen the C header for the Rust runtime lib
1. Install cbindgen

        cargo install cbindgen
2. Execute build command

        cmake --build ./build/ -t runtime_header
3. The generated header will be at : runtime/include/ballerina_rt.h


## Building from source in MacOSX 10.15.0
### Prerequisites
* If you haven’t installed xcode and brew yet, please install them. Following steps assumes that xcode and brew are already installed. 
* `brew install llvm@11 rust python3`
* `brew postinstall python3`
* `pip3 install lit filecheck`

### Build steps
1.  `cd build`
2. `cmake ../ -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm/ -DCMAKE_CXX_STANDARD=14`
3. `make -j`

This will build:
* The Rust runtime : runtime/target/release/libballerina_rt.so
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Run tests
1. Install Java 8 runtime (e.g. JRE 8 package for MacOS)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [jballerina-tools-2.0.0-Preview2-nballerina-r2.zip](https://github.com/ballerina-platform/ballerina-lang/packages/413010?version=2.0.0-Preview2-nballerina-r2)
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
* Create the executable file and execute it

        clang -O0 main.ll
        a.exe

### Run tests
1. Install cbindgen

        cargo install cbindgen

2. Install lit

        pip3 Install lit filecheck

3. Install java and set up JAVA_HOME in your PATH system variable
4. Get this specific Ballerina version : [jballerina-tools-2.0.0-Preview2-nballerina-r2.zip](https://github.com/ballerina-platform/ballerina-lang/packages/413010?version=2.0.0-Preview2-nballerina-r2)
5. Extract Ballerina pack and add the bin folder with the ballerina executable to your PATH system variable
4. Run tests

        msbuild test /p:configuration=Release


## Setting up for Development (Ubuntu 20.04)

* First setup dev environment following the build from source guide for your platform
* Install the following additional dependencies

        sudo apt install gcovr clang-tidy (or a newer version)

### Debug Builds 

By default debug build enables both sanitizers (via gcc or clang) and static analysis (via clang-tidy). To build a debug build use the following command template:


        cmake -DCMAKE_BUILD_TYPE=Release <additional flags> -S . -B build/
        cmake --build ./build/

The valid additional flags are listed below:
* `-DSKIP_SANITIZER=ON` : Don't enable sanitizers
  * `-USKIP_SANITIZER` To unset it
* `-DSKIP_ANALYSE=ON` : Don't enable static analysis
  * `-USKIP_ANALYSE` To unset it
* `-DCLANG_TIDY_OVERRIDE=<compiler name>` : Override default clang-tidy executable name e.g. `-DCLANG_TIDY_OVERRIDE=clang-tidy-11`
* `-DCMAKE_CXX_COMPILER=<compiler name>` : Override default compiler e.g. `-DCMAKE_CXX_COMPILER=clang++-11`, `-DCMAKE_CXX_COMPILER=g++`
* `-DENABLE_COVERAGE=ON` : Enable code coverage
  * Valid only for debug builds
  * Launch code coverage test via `cmake --build ./build/ -t nballerinacc_coverage`
  * `-UENABLE_COVERAGE` To unset it

**Additional notes:**
* Don't use parallel builds with static analysis ON; otherwise the suggestions dumped on stdout will be hard to parse
* To disable certain clang-tidy checks, add them to the list @ line 11 in the `Clang-tidy.cmake` file
* To enable/disable specific sanitizers, edit the wnd of `Sanitizers.cmake` file
