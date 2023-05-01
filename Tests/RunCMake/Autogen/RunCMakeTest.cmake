include(RunCMake)

function(getIncludeFlags)
  message(STATUS "RunCMake_TEST_BINARY_DIR: ${RunCMake_TEST_BINARY_DIR}")
  message(STATUS "RunCMake_SOURCE_DIR: ${RunCMake_SOURCE_DIR}")
  execute_process(COMMAND cmake -S ${RunCMake_SOURCE_DIR}/subdir -B ${RunCMake_BINARY_DIR}/subdir/build
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
  )
  if(WIN32)
    # Set EOL character to \r\n on Windows
    set(CMAKE_EOL "\r\n")
  else()
    # Set EOL character to \n on Unix-based systems
    set(CMAKE_EOL "\n")
  endif()

  # parse the output line by line and check for CMAKE_INCLUDE_SYSTEM_FLAG_CXX
  # and CMAKE_INCLUDE_FLAG_CXX in the output

  string(REPLACE "${CMAKE_EOL}" ";" lines "${output}")

  # Find the lines that contain the variables we are interested in
  foreach(line ${lines})
    if(line MATCHES "CMAKE_INCLUDE_SYSTEM_FLAG_CXX")
      # get the value between the pipes
      string(REGEX MATCH "\\|(.*)\\|" match ${line})
      message(STATUS "CMAKE_INCLUDE_SYSTEM_FLAG_CXX: ${CMAKE_MATCH_1}")
      set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
    if(line MATCHES "CMAKE_INCLUDE_FLAG_CXX")
      # get the value between the pipes
      string(REGEX MATCH "\\|(.*)\\|" match ${line})
      message(STATUS "CMAKE_INCLUDE_FLAG_CXX: ${CMAKE_MATCH_1}")
      set(CMAKE_INCLUDE_FLAG_CXX ${CMAKE_MATCH_1} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

run_cmake(NoQt)
if (DEFINED with_qt_version)
  set(RunCMake_TEST_OPTIONS
    -Dwith_qt_version=${with_qt_version}
    "-DQt${with_qt_version}_DIR:PATH=${Qt${with_qt_version}_DIR}"
    "-DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}"
  )

  run_cmake(QtInFunction)
  run_cmake(QtInFunctionNested)
  run_cmake(QtInFunctionProperty)

  run_cmake(CMP0111-imported-target-full)
  run_cmake(CMP0111-imported-target-libname)
  run_cmake(CMP0111-imported-target-implib-only)

  block()
    set(RunCMake_TEST_BINARY_DIR  ${RunCMake_BINARY_DIR}/MocPredefs-build)
    run_cmake(MocPredefs)
    set(RunCMake_TEST_NO_CLEAN 1)
    run_cmake_command(MocPredefs-build ${CMAKE_COMMAND} --build . --config Debug)
  endblock()

  getIncludeFlags()
  set(OPTIONAL_CHARS "(\"?)")

  if (NOT CMAKE_INCLUDE_SYSTEM_FLAG_CXX STREQUAL "")
    # multi-config generators
    if (${RunCMake_GENERATOR} STREQUAL "Ninja Multi-Config")
      foreach(config IN ITEMS Debug Release RelWithDebInfo)
        block()
          set(RunCMake_TEST_BINARY_DIR  ${RunCMake_BINARY_DIR}/CMP0151-new-multi-${config}-build)
          run_cmake_with_options(CMP0151-new ${RunCMake_TEST_OPTIONS} -DCMAKE_POLICY_DEFAULT_CMP0151=NEW)
          set(RunCMake_TEST_NO_CLEAN 1)
          file(TO_NATIVE_PATH ${RunCMake_TEST_BINARY_DIR} RunCMake_TEST_BINARY_DIR_NATIVE)
          string(CONCAT test_expect_stdout "-*${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${OPTIONAL_CHARS}"
                                           "${RunCMake_TEST_BINARY_DIR_NATIVE}/dummy_autogen/include_${config}")
          set(RunCMake_TEST_EXPECT_stdout ${test_expect_stdout})
          message(STATUS "RunCMake_TEST_EXPECT_stdout: ${RunCMake_TEST_EXPECT_stdout}")
          run_cmake_command(CMP0151-new-multi-${config}-build ${CMAKE_COMMAND} --build . --config ${config} --verbose)
        endblock()
      endforeach()
    endif()

    # single-config generators
    block()
      set(RunCMake_TEST_BINARY_DIR  ${RunCMake_BINARY_DIR}/CMP0151-new-build)
      run_cmake_with_options(CMP0151-new ${RunCMake_TEST_OPTIONS} -DCMAKE_POLICY_DEFAULT_CMP0151=NEW)
      set(RunCMake_TEST_NO_CLEAN 1)
      file(TO_NATIVE_PATH ${RunCMake_TEST_BINARY_DIR} RunCMake_TEST_BINARY_DIR_NATIVE)
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(REPLACE "\\" "\\\\" RunCMake_TEST_BINARY_DIR_NATIVE ${RunCMake_TEST_BINARY_DIR_NATIVE})
      endif()
      string(CONCAT test_expect_stdout "-*${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${OPTIONAL_CHARS}"
                                       "${RunCMake_TEST_BINARY_DIR_NATIVE}/dummy_autogen/include")
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(CONCAT test_expect_stdout ${test_expect_stdout} "_Debug")
      endif()
      set(RunCMake_TEST_EXPECT_stdout ${test_expect_stdout})
      message(STATUS "RunCMake_TEST_EXPECT_stdout: ${RunCMake_TEST_EXPECT_stdout}")
      run_cmake_command(CMP0151-new-build ${CMAKE_COMMAND} --build . --config Debug --verbose)
    endblock()

    block()
      set(RunCMake_TEST_BINARY_DIR  ${RunCMake_BINARY_DIR}/AutogenUseSystemIncludeOn-build)
      run_cmake_with_options(AutogenUseSystemIncludeOn ${RunCMake_TEST_OPTIONS} -DCMAKE_POLICY_DEFAULT_CMP0151=NEW)
      set(RunCMake_TEST_NO_CLEAN 1)
      file(TO_NATIVE_PATH ${RunCMake_TEST_BINARY_DIR} RunCMake_TEST_BINARY_DIR_NATIVE)
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(REPLACE "\\" "\\\\" RunCMake_TEST_BINARY_DIR_NATIVE ${RunCMake_TEST_BINARY_DIR_NATIVE})
      endif()
      string(CONCAT test_expect_stdout "-*${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${OPTIONAL_CHARS}"
                                       "${RunCMake_TEST_BINARY_DIR_NATIVE}/dummy_autogen/include")
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(CONCAT test_expect_stdout ${test_expect_stdout} "_Debug")
      endif()
      set(RunCMake_TEST_EXPECT_stdout ${test_expect_stdout})
      message(STATUS "RunCMake_TEST_EXPECT_stdout: ${RunCMake_TEST_EXPECT_stdout}")
      run_cmake_command(AutogenUseSystemIncludeOn ${CMAKE_COMMAND} --build . --config Debug --verbose)
    endblock()
  endif()

  if (NOT CMAKE_INCLUDE_FLAG_CXX STREQUAL "")
    if (${RunCMake_GENERATOR} STREQUAL "Ninja Multi-Config")
      # multi-config generators
      foreach(config IN ITEMS Debug Release RelWithDebInfo)
        block()
          set(RunCMake_TEST_BINARY_DIR  ${RunCMake_BINARY_DIR}/CMP0151-old-multi-${config}-build)
          run_cmake_with_options(CMP0151-old ${RunCMake_TEST_OPTIONS} -DCMAKE_POLICY_DEFAULT_CMP0151=OLD)
          set(RunCMake_TEST_NO_CLEAN 1)
          file(TO_NATIVE_PATH ${RunCMake_TEST_BINARY_DIR} RunCMake_TEST_BINARY_DIR_NATIVE)
          string(CONCAT test_expect_stdout "-*${CMAKE_INCLUDE_FLAG_CXX}${OPTIONAL_CHARS}"
                                           "${RunCMake_TEST_BINARY_DIR_NATIVE}/dummy_autogen/include_${config}")
          set(RunCMake_TEST_EXPECT_stdout ${test_expect_stdout})
          message(STATUS "RunCMake_TEST_EXPECT_stdout: ${RunCMake_TEST_EXPECT_stdout}")
          run_cmake_command(CMP0151-old-multi-${config}-build ${CMAKE_COMMAND} --build . --config ${config} --verbose)
        endblock()
      endforeach()
    endif()

    # single-config generators
    block()
      set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/CMP0151-old-build)
      run_cmake_with_options(CMP0151-old ${RunCMake_TEST_OPTIONS} -DCMAKE_POLICY_DEFAULT_CMP0151=OLD)
      set(RunCMake_TEST_NO_CLEAN 1)
      file(TO_NATIVE_PATH ${RunCMake_TEST_BINARY_DIR} RunCMake_TEST_BINARY_DIR_NATIVE)
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(REPLACE "\\" "\\\\" RunCMake_TEST_BINARY_DIR_NATIVE ${RunCMake_TEST_BINARY_DIR_NATIVE})
      endif()
      string(CONCAT test_expect_stdout "-*${CMAKE_INCLUDE_FLAG_CXX}${OPTIONAL_CHARS}"
                                       "${RunCMake_TEST_BINARY_DIR_NATIVE}/dummy_autogen/include")
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(CONCAT test_expect_stdout ${test_expect_stdout} "_Debug")
      endif()
      set(RunCMake_TEST_EXPECT_stdout ${test_expect_stdout})
      message(STATUS "RunCMake_TEST_EXPECT_stdout: ${RunCMake_TEST_EXPECT_stdout}")
      run_cmake_command(CMP0151-old-build ${CMAKE_COMMAND} --build . --config Debug --verbose)
    endblock()

    block()
      set(RunCMake_TEST_BINARY_DIR  ${RunCMake_BINARY_DIR}/AutogenUseSystemIncludeOff-build)
      run_cmake_with_options(AutogenUseSystemIncludeOff ${RunCMake_TEST_OPTIONS} -DCMAKE_POLICY_DEFAULT_CMP0151=NEW)
      set(RunCMake_TEST_NO_CLEAN 1)
      file(TO_NATIVE_PATH ${RunCMake_TEST_BINARY_DIR} RunCMake_TEST_BINARY_DIR_NATIVE)
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(REPLACE "\\" "\\\\" RunCMake_TEST_BINARY_DIR_NATIVE ${RunCMake_TEST_BINARY_DIR_NATIVE})
      endif()
      string(CONCAT test_expect_stdout "-*${CMAKE_INCLUDE_FLAG_CXX}${OPTIONAL_CHARS}"
                                       "${RunCMake_TEST_BINARY_DIR_NATIVE}/dummy_autogen/include")
      if (${RunCMake_GENERATOR} STREQUAL "Xcode")
        string(CONCAT test_expect_stdout ${test_expect_stdout} "_Debug")
      endif()
      set(RunCMake_TEST_EXPECT_stdout ${test_expect_stdout})
      message(STATUS "RunCMake_TEST_EXPECT_stdout: ${RunCMake_TEST_EXPECT_stdout}")
      run_cmake_command(AutogenUseSystemIncludeOff ${CMAKE_COMMAND} --build . --config Debug --verbose)
    endblock()
    unset(EXPECTED_STDOUT)
  endif()
endif()
