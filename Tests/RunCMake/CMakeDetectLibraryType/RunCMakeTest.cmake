include(RunCMake)

function (run_ByContent)
  # Use a single build tree for a few tests without cleaning.
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/ByContent)
  set(RunCMake_TEST_NO_CLEAN 1)
  if(NOT RunCMake_GENERATOR_IS_MULTI_CONFIG)
    set(RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=Debug)
  endif()
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")
  run_cmake(ByContent)
  run_cmake_command(ByContent-build ${CMAKE_COMMAND} --build . --config Debug)
endfunction ()

run_ByContent()
# Non-mingw toolchains on Windows require content checks.
if (NOT WIN32 OR MINGW)
  run_cmake(ByExtension)
endif ()

# Argument parsing tests
run_cmake(ExtraArguments)
# run_cmake(EmptyPath) # FIXME: Not testable with the CMake implementation at the moment.
run_cmake(MissingPath)
