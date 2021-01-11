include(RunCMake)

function(run_cmake_and_verify_after_build case)
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/${case}-build")
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake(${case})
  run_cmake_command("${case}-build" ${CMAKE_COMMAND} --build .)
  unset(RunCMake_TEST_NO_CLEAN)
  unset(RunCMake_TEST_BINARY_DIR)
endfunction()

run_cmake_and_verify_after_build(ArtifactOutputDirs)
