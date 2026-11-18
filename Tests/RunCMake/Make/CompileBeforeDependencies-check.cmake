# The dependent target must get a compile-only driver that waits only
# for the dependency's custom-command outputs (via .../depend), while the
# link driver waits for the full dependency (mirrors Ninja issue #15555,
# including plain non-CODEGEN add_custom_command() outputs).
file(READ "${RunCMake_TEST_BINARY_DIR}/CMakeFiles/Makefile2" make2)
if(NOT make2 MATCHES "CMakeFiles/main\\.dir/depend:[^\n]*CMakeFiles/dep\\.dir/depend")
  string(APPEND RunCMake_TEST_FAILED
    "main.dir/depend does not depend on dep.dir/depend:\n${make2}\n")
endif()
if(NOT make2 MATCHES "CMakeFiles/main\\.dir/compile:[^\n]*CMakeFiles/main\\.dir/depend")
  string(APPEND RunCMake_TEST_FAILED
    "main.dir/compile does not depend on main.dir/depend:\n${make2}\n")
endif()
if(NOT make2 MATCHES "CMakeFiles/main\\.dir/all:[^\n]*CMakeFiles/main\\.dir/compile")
  string(APPEND RunCMake_TEST_FAILED
    "main.dir/all does not depend on main.dir/compile:\n${make2}\n")
endif()
if(NOT make2 MATCHES "CMakeFiles/main\\.dir/all:[^\n]*CMakeFiles/dep\\.dir/all")
  string(APPEND RunCMake_TEST_FAILED
    "main.dir/all does not depend on dep.dir/all:\n${make2}\n")
endif()
file(READ "${RunCMake_TEST_BINARY_DIR}/CMakeFiles/main.dir/build.make" buildmake)
if(NOT buildmake MATCHES "CMakeFiles/main\\.dir/compile:")
  string(APPEND RunCMake_TEST_FAILED
    "build.make has no main.dir/compile driver:\n${buildmake}\n")
endif()

# Building the compile driver must not require the dependency to link.
# Direct Makefile2 invocation uses -f, portable to GNU/Borland/Watcom
# makes; skip on NMake whose flag is /F.
if(NOT RunCMake_GENERATOR MATCHES "NMake")
  execute_process(
    COMMAND "${RunCMake_MAKE_PROGRAM}" -f CMakeFiles/Makefile2 "CMakeFiles/main.dir/compile"
    WORKING_DIRECTORY "${RunCMake_TEST_BINARY_DIR}"
    OUTPUT_VARIABLE compile_stdout
    ERROR_VARIABLE compile_stderr
    RESULT_VARIABLE compile_result)
  if(NOT compile_result EQUAL 0)
    string(APPEND RunCMake_TEST_FAILED
      "compile driver failed:\n${compile_stdout}\n${compile_stderr}\n")
  endif()
  file(GLOB dep_lib "${RunCMake_TEST_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}dep${CMAKE_SHARED_LIBRARY_SUFFIX}*")
  if(dep_lib)
    string(APPEND RunCMake_TEST_FAILED
      "dep library was built when requesting main.dir/compile: ${dep_lib}\n")
  endif()
endif()
