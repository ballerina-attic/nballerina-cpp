# Add Rust Runtime
include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/runtime)
ExternalProject_Add(
    nballerina_rt
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/runtime"
    INSTALL_COMMAND cbindgen --config cbindgen.toml --crate nBallerinaRT --output include/nballerina_rt.h
    LOG_BUILD ON
    BUILD_ALWAYS TRUE)