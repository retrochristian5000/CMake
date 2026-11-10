include(RunCMake)

macro(run_cmake_build test)
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/${test}-build")
  run_cmake(${test})
  execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${RunCMake_TEST_BINARY_DIR}"
    RESULT_VARIABLE _build_result
    )
  if(NOT _build_result EQUAL 0)
    message(SEND_ERROR "Building ${test} failed with result ${_build_result}")
  endif()
endmacro()

set(RunCMake_TEST_OPTIONS "-DBISON_EXECUTABLE=${BISON_EXECUTABLE}")

run_cmake(Defaults)
run_cmake(DefinesFile)
run_cmake(Options)
run_cmake(Verbose)
run_cmake(CMP0088-OLD)
run_cmake(BadArgs)

block()
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/Build-build)
  run_cmake(Build)
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(Build-build ${CMAKE_COMMAND} --build .)
endblock()

run_cmake_build(header-paths-new)
run_cmake(header-paths-old)
