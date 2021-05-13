project(c_rt)
cmake_minimum_required(VERSION 3.16)

include_directories(.)

add_library( ballerina_crt STATIC src/balmap.c src/lto.c)

set(CMAKE_CXX_FLAGS "-fuse-ld=lld-11")
set(LTO_FLAGS --target=x86_64-unknown-linux-gnu -flto=thin -Wno-override-module)
target_compile_options(ballerina_crt PUBLIC "$<$<CONFIG:DEBUG>:${LTO_FLAGS}>")
target_compile_options(ballerina_crt PUBLIC "$<$<CONFIG:RELEASE>:${LTO_FLAGS}>")

include(GoogleTest)
add_executable(check_crt test/main.cpp)
target_link_libraries(check_crt ballerina_crt gtest_main gtest pthread )
set(TESTAPP_FLAGS -Wno-unused-command-line-argument)
target_compile_options(check_crt PUBLIC "$<$<CONFIG:DEBUG>:${TESTAPP_FLAGS}>")
target_compile_options(check_crt PUBLIC "$<$<CONFIG:RELEASE>:${TESTAPP_FLAGS}>")

gtest_discover_tests(check_crt)

set(UNIT_TEST check_crt)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "Run tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${UNIT_TEST}
)
