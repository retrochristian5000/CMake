include(RunCMake)

function(run_compile_warn test)
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${test}-build)
  set(RunCMake_TEST_NO_CLEAN 1)
  set(RunCMake_TEST_OUTPUT_MERGE 1)
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")
  run_cmake(${test})
  run_cmake_command(${test}-Build ${CMAKE_COMMAND} --build . ${verbose_args})
endfunction()

run_compile_warn(WerrorOn)
run_compile_warn(WerrorOff)
