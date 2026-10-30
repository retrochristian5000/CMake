include(RunCMake)

if(CMake_TEST_CLANG_COVERAGE)
  block()
    set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/ClangCoverage-build)
    if(NOT RunCMake_GENERATOR_IS_MULTI_CONFIG)
      list(APPEND RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=Debug)
    endif()
    run_cmake(ClangCoverage)
    set(RunCMake_TEST_NO_CLEAN 1)
    run_cmake_command(ClangCoverage-build ${CMAKE_COMMAND} --build . --config Debug)
    run_cmake_command(ClangCoverage-ctest ${CMAKE_CTEST_COMMAND} -C Debug -VV)
    run_cmake_command(ClangCoverage-coverage ${CMAKE_CTEST_COMMAND} -D ExperimentalCoverage -C Debug -VV)
  endblock()
endif()

function(run_CMakeListFileCoverage case)
  run_cmake(${case})
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(${case}-ctest ${CMAKE_CTEST_COMMAND} -C Debug -VV)
  run_cmake_command(${case}-coverage ${CMAKE_CTEST_COMMAND} -D ExperimentalCoverage -C Debug -VV)
endfunction()

block()
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/CMakeListFileCoverage-build)
  run_CMakeListFileCoverage(CMakeListFileCoverage)
endblock()

# A build tree inside the source tree.
block()
  set(src ${RunCMake_BINARY_DIR}/nested-src)
  file(REMOVE_RECURSE "${src}")
  file(MAKE_DIRECTORY "${src}")
  file(COPY_FILE ${RunCMake_SOURCE_DIR}/CMakeLists.txt ${src}/CMakeLists.txt)
  file(COPY_FILE ${RunCMake_SOURCE_DIR}/Shared.cmake ${src}/Shared.cmake)
  file(COPY_FILE ${RunCMake_SOURCE_DIR}/CMakeListFileCoverage.cmake
    ${src}/CMakeListFileCoverage-nested.cmake)
  set(RunCMake_TEST_SOURCE_DIR ${src})
  set(RunCMake_TEST_BINARY_DIR ${src}/build)
  run_CMakeListFileCoverage(CMakeListFileCoverage-nested)
endblock()

# A line past the end of the file, and a truncated file.
block()
  set(tree ${RunCMake_BINARY_DIR}/CMakeListFileCoverage-range)
  set(src ${RunCMake_BINARY_DIR}/CMakeListFileCoverage-range-src)
  file(REMOVE_RECURSE "${tree}" "${src}")
  file(WRITE "${src}/three.cmake" "set(a 1)\nset(b 2)\nset(c 3)\n")
  file(WRITE "${tree}/DartConfiguration.tcl" "
SourceDirectory: ${src}
BuildDirectory: ${tree}
")
  file(WRITE "${tree}/bad.cmake-coverage.info"
    "SF:${src}/three.cmake\nDA:10,1\nend_of_record\n")
  file(WRITE "${tree}/trunc.cmake-coverage.info"
    "SF:${src}/three.cmake\nDA:1,1\n")
  set(RunCMake_TEST_BINARY_DIR ${tree})
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(CMakeListFileCoverage-range ${CMAKE_CTEST_COMMAND} -D ExperimentalCoverage -VV)
endblock()

# Only a record with no lines and no functions: no coverage data.
block()
  set(tree ${RunCMake_BINARY_DIR}/CMakeListFileCoverage-empty)
  set(src ${RunCMake_BINARY_DIR}/CMakeListFileCoverage-empty-src)
  file(REMOVE_RECURSE "${tree}" "${src}")
  file(WRITE "${src}/comments.cmake" "# Only a comment.\n")
  file(WRITE "${tree}/DartConfiguration.tcl" "
SourceDirectory: ${src}
BuildDirectory: ${tree}
")
  file(WRITE "${tree}/empty.cmake-coverage.info"
    "SF:${src}/comments.cmake\nend_of_record\n")
  set(RunCMake_TEST_BINARY_DIR ${tree})
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(CMakeListFileCoverage-empty ${CMAKE_CTEST_COMMAND} -D ExperimentalCoverage)
endblock()
