if(CLANG_TIDY_OVERRIDE)
    find_program( CLANG_TIDY_EXE NAMES "${CLANG_TIDY_OVERRIDE}" DOC "Path to clang-tidy executable" )
else()
    find_program( CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )
endif()

if(NOT CLANG_TIDY_EXE) 
    message(STATUS "clang-tidy not found.") 
else() 
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}") 
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "--use-color=1" "-checks=*,-fuchsia-*,-google-*,-llvmlibc-*,\
        -modernize-use-trailing-return-type, -hicpp-named-parameter, -readability-named-parameter")
endif()
