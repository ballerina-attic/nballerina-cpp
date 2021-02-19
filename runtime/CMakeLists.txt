# Add Rust Runtime
include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/runtime)

# Add cargo build instructions for Rust based runtime
ExternalProject_Add(
    BallerinaRT
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/runtime"
    INSTALL_COMMAND ""
    LOG_BUILD ON
    BUILD_ALWAYS TRUE)

# Add seperate (optional) target to generate header file for runtime lib using cbindgen
add_custom_target(runtime_header
  COMMENT "Generate C header file for runtime lib"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
  COMMAND cbindgen --config cbindgen.toml --crate BallerinaRT --output include/ballerina_rt.h
  DEPENDS BallerinaRT)

add_dependencies(runtime_header BallerinaRT)