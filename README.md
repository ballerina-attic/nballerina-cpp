# nBallerina
Translate Ballerina IR to LLVM IR.

## Building from source in Ubuntu 20.04
### Prerequisites
* `sudo apt install build-essential llvm-11-dev lld-11 cmake cargo python3-pip libgtest-dev`
* `pip3 install lit filecheck`

### Build steps
1. `cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-11 -DCMAKE_C_COMPILER=clang-11 -S . -B build/`
2. `cmake --build ./build/ -- -j`

This will build:
* The Rust runtime : build/runtime/rust_rt/release/libballerina_rt.a
* The C runtime : build/runtime/c_rt/libballerina_crt.a
* The nballerinacc (BIR to LLVM IR converter) app : build/nballerinacc

### Usage
* Run nballerinacc against a BIR dump file (using `bal build -dump-bir-file=<output> <input>`) to generate the .ll LLVM IR file
 
        ./nballerinacc <bir dump file path>
* The .ll file can be compiled into an executable using clang and the compiled runtime library
 
        clang-11 --target=x86_64-unknown-linux-gnu -c -O3 -flto=thin -Wno-override-module -o $filename.o $filename.ll
        clang-11 -flto=thin -fuse-ld=lld-11 -Lruntime/rust_rt/release/ -Lruntime/c_rt/ -lballerina_rt -lballerina_crt -lpthread -ldl -o $filename.out -O3 $filename.o 

### Run tests
1. Install Java 11 runtime (e.g. openjdk-11-jre on ubuntu)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [2.0.0-alpha8-20210419-135100-df7a4e3f.zip](https://github.com/ballerina-platform/ballerina-lang/packages/413010?version=2.0.0-alpha8-20210419-135100-df7a4e3f))
4. Extract Ballerina pack and add the `bin` folder with the `bal` executable to your `PATH` system variable 
5. Run:

        cmake --build ./build/ -t check

### To codegen the C header for the Rust runtime lib
1. Install cbindgen

        cargo install cbindgen
2. Execute build command

        cmake --build ./build/ -t runtime_header
3. The generated header will be at : runtime/include/ballerina_rt.h

### Check whether LTO is inlining runtime functions
1. Execute command

        cmake --build ./build/ -t check_lto


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
1. Install Java 11 runtime (e.g. JRE 11 package for MacOS)
2. Create `JAVA_HOME` environment variable and point it to the Java install dir
3. Get this specific Ballerina version : [2.0.0-alpha8-20210419-135100-df7a4e3f.zip](https://github.com/ballerina-platform/ballerina-lang/packages/413010?version=2.0.0-alpha8-20210419-135100-df7a4e3f))
4. Extract Ballerina pack and add the `bin` folder with the `bal` executable to your `PATH` system variable 
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
    
        cmake -DCMAKE_BUILD_TYPE=Release -Thost=x64 -G "Visual Studio 16 2019" -A x64 -DLLVM_USE_CRT_RELEASE=MT -DCMAKE_INSTALL_PREFIX=install -S . -B build\
        cmake --build .\build -t install -- /p:configuration=Release

   
### Build steps
Clone the nballerina source and run below commands.
1. `cmake -DLLVM_DIR=<LLVM_INSTALL_PATH>\lib\cmake\llvm -S . -B .\build`
2. `cmake --build .\build --config Release`

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

3. Install java 11 and set up JAVA_HOME in your PATH system variable
4. Get this specific Ballerina version : [2.0.0-alpha8-20210419-135100-df7a4e3f.zip](https://github.com/ballerina-platform/ballerina-lang/packages/413010?version=2.0.0-alpha8-20210419-135100-df7a4e3f))
5. Extract Ballerina pack and add the bin folder with the ballerina executable to your PATH system variable
6. Run tests

        msbuild test /p:configuration=Release


## Setting up for Development (Ubuntu 20.04)

* First setup dev environment following the build from source guide for your platform
* Install the following additional dependencies

        sudo apt install gcovr clang-tidy (or a newer version)

### Debug Builds 

By default debug build enables both sanitizers (via gcc or clang) and static analysis (via clang-tidy). To build a debug build use the following command template:


        cmake -DCMAKE_BUILD_TYPE=Release <options> -S . -B build/
        cmake --build ./build/

Valid options:
* `-DENABLE_SANITIZER=ON/OFF` : Enable sanitizers for GCC and Clang++ (Default `ON`)
  * Valid only for debug builds
* `-DENABLE_ANALYSE=ON/OFF` : Enable static analysis if clang-tidy is detected (Default `ON`)
  * Valid only for debug builds
* `-DENABLE_COVERAGE=ON/OFF` : Enable code coverage (Default `OFF`)
  * Valid only for debug builds
  * Launch code coverage test via `cd build; cmake --build . -t nballerinacc_coverage`
* `-DSKIP_BIR_GEN=ON` : Skip the execution of `ballerina build --dump-file` for check target, if dump files already exists
  * Set this to speed up LIT test execution. 
  * `-USKIP_BIR_GEN` To unset it
* `-DCLANG_TIDY_OVERRIDE=<compiler name>` : Override default clang-tidy executable name e.g. `-DCLANG_TIDY_OVERRIDE=clang-tidy-11`
  * `-DCLANG_TIDY_OVERRIDE` To unset it
* `-DCHECK_FILTER="--filter=<regex>"` : Filter tests to run. e.g. `-DCHECK_FILTER="--filter=\"simpleMain.bal\""`, `-DCHECK_FILTER="--filter=\"functions\""`
  * `-UCHECK_FILTER` To unset it
* `-DCMAKE_CXX_COMPILER=<compiler name>` : Override default compiler e.g. `-DCMAKE_CXX_COMPILER=clang++-11`, `-DCMAKE_CXX_COMPILER=g++`
  * `-UCMAKE_CXX_COMPILER` To unset it

**Additional notes:**
* Recommend not to use parallel builds with static analysis ON; otherwise the suggestions dumped on stdout will be hard to parse
* To disable certain clang-tidy checks, add them to the list @ line 11 in the `Clang-tidy.cmake` file
* To enable/disable specific sanitizers, edit the `add_sanitizers` macro in `Sanitizers.cmake` file