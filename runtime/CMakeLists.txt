# Add Rust Runtime
include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/runtime)
ExternalProject_Add(
    BallerinaRT
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/runtime"
    INSTALL_COMMAND cbindgen --config cbindgen.toml --crate BallerinaRT --output include/ballerina_rt.h
    LOG_BUILD ON
    BUILD_ALWAYS TRUE)