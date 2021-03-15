# @brief Set variables indicating if mode is Debug or Release
# - The mode might be neither of them
macro(set_debug_booleans)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(DEBUG_MODE ON)
        set(NOT_DEBUG_MODE OFF)
        set(RELEASE_MODE OFF)
        set(NOT_RELEASE_MODE ON)
    else ()
        set(DEBUG_MODE OFF)
        set(NOT_DEBUG_MODE ON)
        set(RELEASE_MODE ON)
        set(NOT_RELEASE_MODE OFF)
    endif ()
endmacro()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
