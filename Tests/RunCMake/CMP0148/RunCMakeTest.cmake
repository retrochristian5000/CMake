include(RunCMake)

function(test_CMP0148 val)
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${val}-build)
  run_cmake(CMP0148-${val})
  set(RunCMake_TEST_NO_CLEAN TRUE)
  # Some git versions write clone messages to stderr. These would cause the
  # test to fail, so we need to merge them into stdout.
  set(RunCMake_TEST_OUTPUT_MERGE TRUE)
  run_cmake_command(CMP0148-${val}-build ${CMAKE_COMMAND} --build .)
endfunction()

test_CMP0148(WARN)
test_CMP0148(OLD)
test_CMP0148(NEW)

run_cmake_script(CMP0148-NEW-resolve)
