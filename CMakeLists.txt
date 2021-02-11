cmake_minimum_required(VERSION 3.2)
if (NOT UNIX)
    cmake_policy(SET CMP0091 NEW)
endif()
project(nballerina VERSION 0.0.1 LANGUAGES CXX)

# Find and loacd LLVM components
find_package(LLVM 11.1.0 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
add_definitions(${LLVM_DEFINITIONS})

# Include and lib directories
include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/runtime/include)
link_directories(${CMAKE_SOURCE_DIR}/runtime/target/release)

# Add runtime project
add_subdirectory(runtime)

# Add nballerinacc
file(GLOB SOURCES
${PROJECT_SOURCE_DIR}/compiler/*.cpp
)
add_executable(nballerinacc ${SOURCES})

add_dependencies(nballerinacc BallerinaRT)

if (UNIX) 
    target_link_libraries(nballerinacc PRIVATE LLVM-11)
else ()
    set_property(TARGET nballerinacc PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_link_libraries(nballerinacc PRIVATE LLVM-C
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitstreamReader
    LLVMCodeGen
    LLVMCore
    LLVMRemarks
    LLVMSupport)
endif()

# Set extra warnings for nballerinacc
set(DEBUG_OPTIONS -Wall)
set(RELEASE_OPTIONS -Wall)
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

# Add LIT tests
add_subdirectory(test)
