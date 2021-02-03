cmake_minimum_required(VERSION 3.2)
project(nballerina VERSION 0.0.1 LANGUAGES CXX)

# Find and loacd LLVM components
find_package(LLVM 11 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add Rust Runtime
include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/runtime)
ExternalProject_Add(
    nballerinart
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/runtime"
    INSTALL_COMMAND ""
    LOG_BUILD ON)

# Define nabllerinacc target and add src files and libs
file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/BIR2llvmIR/*.cpp
)
add_executable(nballerinacc ${SOURCES})
add_dependencies(nballerinacc nballerinart)
target_link_libraries(nballerinacc PRIVATE LLVM-11)

# Set extra warnings
#set(DEBUG_OPTIONS -Wall -Wextra -pedantic)
#set(RELEASE_OPTIONS -Wall -Werror)
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

# Add LIT tests
add_subdirectory(test)
