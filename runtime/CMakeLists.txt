if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_CMD cargo build --verbose)
else ()
    set(CARGO_CMD cargo build --release --verbose)
endif ()

# Flags to enable C++/Rust LTO
set(RUST_FLAGS "-Clinker-plugin-lto" "-Clinker=clang-11" "-Clink-arg=-fuse-ld=lld-11")

# Add cargo build instructions for Rust based runtime
add_custom_target(libballerina_rt
  COMMENT "Generate static runtime lib"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} RUSTFLAGS="${RUST_FLAGS}" ${CARGO_CMD})

# Add seperate (optional) target to generate header file for runtime lib using cbindgen
add_custom_target(runtime_header
  COMMENT "Generate C header file for runtime lib"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
  COMMAND cbindgen --config cbindgen.toml --crate BallerinaRT --output include/ballerina_rt.h
  DEPENDS libballerina_rt)

add_dependencies(runtime_header libballerina_rt)
