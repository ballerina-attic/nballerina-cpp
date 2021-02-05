cmake_minimum_required(VERSION 3.2)
project(nballerina VERSION 0.0.1 LANGUAGES CXX)

# Find and loacd LLVM components
find_package(LLVM 11 REQUIRED CONFIG)
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
add_dependencies(nballerinacc nballerina_rt)
target_link_libraries(nballerinacc PRIVATE LLVM-11)

# Set extra warnings for nballerinacc
set(DEBUG_OPTIONS -Wall)
set(RELEASE_OPTIONS -Wall)
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

# Add LIT tests
add_subdirectory(test)