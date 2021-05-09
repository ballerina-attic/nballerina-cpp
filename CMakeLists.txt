cmake_minimum_required(VERSION 3.15)
if (NOT UNIX)
    cmake_policy(SET CMP0091 NEW)
endif()

project(nballerina VERSION 0.0.1 LANGUAGES CXX)

# Add runtime project
add_subdirectory(runtime)

# Add compiler
add_subdirectory(compiler)

# Add LIT tests
add_subdirectory(test)

# tell CMake to generate config.h
configure_file (
  "${PROJECT_SOURCE_DIR}/common/config.h.in"
  "${PROJECT_SOURCE_DIR}/compiler/include/codegen/config.h"
 )
 
