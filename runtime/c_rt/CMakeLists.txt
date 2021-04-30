project(c_rt)

include_directories(.)

ADD_LIBRARY( ballerina_crt STATIC src/balmap.c )
ADD_EXECUTABLE( testapp test/main.c)
TARGET_LINK_LIBRARIES( testapp ballerina_crt )
