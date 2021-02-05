set(java_path "$ENV{JAVA_HOME}")
configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)

add_custom_target(check
  COMMAND ${CMAKE_SOURCE_DIR}/test/run_lit_tests.py "${CMAKE_CURRENT_BINARY_DIR}" -v
  DEPENDS nballerinacc)
