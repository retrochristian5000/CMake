include(RunCMake)

function(configure_and_build target)
  set(RunCMake_TEST_NO_CLEAN 1)
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${target}-build)
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  run_cmake(${target})
  if (RunCMake_GENERATOR_IS_MULTI_CONFIG)
    run_cmake_command(${target}-build ${CMAKE_COMMAND} --build "${RunCMake_TEST_BINARY_DIR}" --config Debug)
    run_cmake_command(${target}-build ${CMAKE_COMMAND} --build "${RunCMake_TEST_BINARY_DIR}" --config Release)
  else()
    run_cmake_command(${target}-build ${CMAKE_COMMAND} --build "${RunCMake_TEST_BINARY_DIR}")
  endif()
endfunction()

configure_and_build(C)
configure_and_build(CXX)

if(CMake_TEST_CUDA)
  configure_and_build(CUDA)
endif()
