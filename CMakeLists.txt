cmake_minimum_required(VERSION 3.2)
project(nballerina VERSION 0.0.1 LANGUAGES CXX)

# Find and loacd LLVM components
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Define nabllerinacc target and add src files and libs
file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/BIR2llvmIR/*.cpp
)
add_executable(nballerinacc ${SOURCES})
target_link_libraries(nballerinacc PRIVATE LLVM-11)

# Add LIT tests
add_subdirectory(test)