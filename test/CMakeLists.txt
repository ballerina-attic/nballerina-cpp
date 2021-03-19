set(java_path "$ENV{JAVA_HOME}")
configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)

set (PYTHON_EXE "python3")
if (NOT UNIX)
    set (PYTHON_EXE "python")
endif()

add_custom_target(check
  COMMAND ${PYTHON_EXE} ${CMAKE_SOURCE_DIR}/test/run_lit_tests.py "${CMAKE_CURRENT_BINARY_DIR}" -v
  DEPENDS nballerinacc)
  