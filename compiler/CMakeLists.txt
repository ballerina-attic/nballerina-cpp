include(Sanitizers.cmake)
include(Clang-tidy.cmake)
include(Misc.cmake)

set_debug_booleans()

# Find and loacd LLVM components
find_package(LLVM 11 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
add_definitions(${LLVM_DEFINITIONS})

# Include and lib directories
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS}) 
include_directories(${CMAKE_SOURCE_DIR}/compiler/include)
link_directories(${CMAKE_SOURCE_DIR}/runtime/target/release)

# Add source files
file(GLOB SOURCES
${PROJECT_SOURCE_DIR}/compiler/*.cpp
)
file(GLOB CODEGEN
${PROJECT_SOURCE_DIR}/compiler/codegen/*.cpp
)
add_executable(nballerinacc ${SOURCES} ${CODEGEN})

# Output to root of build directory
set_target_properties( nballerinacc
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# We want to build nballerinacc after the Runtime
add_dependencies(nballerinacc BallerinaRT)

# Find and like LLVM static libs
llvm_map_components_to_libnames(llvm_libs codegen)
target_link_libraries(nballerinacc PRIVATE ${llvm_libs})

# Use C++17 standard
target_compile_features(nballerinacc PRIVATE cxx_std_17)

# Set MSVC only flag to statically link LLVM libs
if (MSVC)
    set_property(TARGET nballerinacc PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Set compiler specific extra warnings for nballerinacc
if (MSVC)
    set(DEBUG_OPTIONS /W4)
    set(RELEASE_OPTIONS /W3 /WX)
else()
    set(DEBUG_OPTIONS -Wall -Wextra -pedantic)
    set(RELEASE_OPTIONS -Wall -Werror)
endif()
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(nballerinacc PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

if(DEBUG_MODE)
    # Enable Static analysis for debug builds
    if(CLANG_TIDY_EXE AND NOT(SKIP_ANALYSE)) 
        message(STATUS "Enabling clang-tidy checks") 
        set_target_properties(nballerinacc PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}" )
    endif()
    # Enable sanitizers for debug builds
    if(NOT SKIP_SANITIZER)
        add_sanitizers()
    endif()
else()
    # Enable Link time optimizations for Release builds
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set(TARGET nballerinacc CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "LTO is enabled")
    else()
        message(SEND_ERROR "LTO is not supported: ${output}")
    endif()
endif()
