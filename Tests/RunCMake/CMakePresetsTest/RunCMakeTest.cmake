include(RunCMake)

function(run_cmake_test_presets name CMakePresetsTest_CONFIGURE_PRESETS CMakePresetsTest_BUILD_PRESETS CMakePresetsTest_TEST_PRESETS)
  set(RunCMake_TEST_SOURCE_DIR "${RunCMake_BINARY_DIR}/${name}")
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_TEST_SOURCE_DIR}/build")
  set(RunCMake_TEST_COMMAND_WORKING_DIRECTORY "${RunCMake_TEST_SOURCE_DIR}")

  set(RunCMake_TEST "${name}")
  set(RunCMake_TEST_NO_CLEAN TRUE)

  message(STATUS "SOURCE_DIR: ${RunCMake_TEST_SOURCE_DIR}")
  message(STATUS "BINARY_DIR: ${RunCMake_TEST_BINARY_DIR}")
  message(STATUS "RunCMake_SOURCE_DIR: ${RunCMake_SOURCE_DIR}")
  message(STATUS "RunCMake_TEST: ${RunCMake_TEST}")

  file(REMOVE_RECURSE "${RunCMake_TEST_SOURCE_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")
  configure_file("${RunCMake_SOURCE_DIR}/CMakeLists.txt.in" "${RunCMake_TEST_SOURCE_DIR}/CMakeLists.txt" @ONLY)

  if(NOT CMakePresetsTest_FILE)
    set(CMakePresetsTest_FILE "${RunCMake_SOURCE_DIR}/${name}.json.in")
  endif()
  if(EXISTS "${CMakePresetsTest_FILE}")
    configure_file("${CMakePresetsTest_FILE}" "${RunCMake_TEST_SOURCE_DIR}/CMakePresets.json" @ONLY)
  endif()

  if(NOT CMakeUserPresets_FILE)
    set(CMakeUserPresets_FILE "${RunCMake_SOURCE_DIR}/${name}User.json.in")
  endif()
  if(EXISTS "${CMakeUserPresets_FILE}")
    configure_file("${CMakeUserPresets_FILE}" "${RunCMake_TEST_SOURCE_DIR}/CMakeUserPresets.json" @ONLY)
  endif()

  foreach(ASSET ${CMakePresetsTest_ASSETS})
    configure_file("${RunCMake_SOURCE_DIR}/${ASSET}" "${RunCMake_TEST_SOURCE_DIR}" COPYONLY)
  endforeach()

  if (NOT CMakePresetsTest_NO_CONFIGURE)
    foreach(CONFIGURE_PRESET ${CMakePresetsTest_CONFIGURE_PRESETS})
      set(CMakePresetsTest_CONFIGURE_OUTPUT_FILE
        "${RunCMake_TEST_COMMAND_WORKING_DIRECTORY}/configure-${name}-${CONFIGURE_PRESET}.txt")

      execute_process(COMMAND "${CMAKE_COMMAND}"
        "--preset" "${CONFIGURE_PRESET}"
        WORKING_DIRECTORY "${RunCMake_TEST_COMMAND_WORKING_DIRECTORY}"
        RESULT_VARIABLE RESULT
        OUTPUT_FILE "${CMakePresetsTest_CONFIGURE_OUTPUT_FILE}"
        ERROR_FILE "${CMakePresetsTest_CONFIGURE_OUTPUT_FILE}")
      if(RESULT)
        message(FATAL_ERROR "Failed to configure project for test '${name}' (see ${CMakePresetsTest_CONFIGURE_OUTPUT_FILE})")
      endif()
    endforeach()
  endif()

  if (NOT CMakePresetsTest_NO_BUILD)
    foreach(BUILD_PRESET ${CMakePresetsTest_BUILD_PRESETS})
      set(CMakePresetsTest_BUILD_OUTPUT_FILE
        "${RunCMake_TEST_COMMAND_WORKING_DIRECTORY}/build-${name}-${BUILD_PRESET}.txt")

      execute_process(COMMAND "${CMAKE_COMMAND}"
        "--build"
        "--preset" "${BUILD_PRESET}"
        WORKING_DIRECTORY "${RunCMake_TEST_COMMAND_WORKING_DIRECTORY}"
        RESULT_VARIABLE RESULT
        OUTPUT_FILE "${CMakePresetsTest_BUILD_OUTPUT_FILE}"
        ERROR_FILE "${CMakePresetsTest_BUILD_OUTPUT_FILE}")
      if(RESULT)
        message(FATAL_ERROR "Failed to build project for test '${name}' (see ${CMakePresetsTest_BUILD_OUTPUT_FILE})")
      endif()
    endforeach()
  endif()

  foreach(TEST_PRESET ${CMakePresetsTest_TEST_PRESETS})
    if (EXISTS "${RunCMake_SOURCE_DIR}/${name}-${TEST_PRESET}-check.cmake")
      set(RunCMake-check-file "${name}-${TEST_PRESET}-check.cmake")
    else()
      set(RunCMake-check-file "check.cmake")
    endif()
    set(RunCMake-stdout-file "${name}-${TEST_PRESET}-stdout.txt")
    set(RunCMake-stderr-file "${name}-${TEST_PRESET}-stderr.txt")

    set(RunCMake_TEST_COMMAND ${CMAKE_CTEST_COMMAND}
      "--preset" "${TEST_PRESET}"
      ${ARGN})

    run_cmake(${name})
  endforeach()
endfunction()

set(CMakePresets_SCHEMA_EXPECTED_RESULT 0)
set(CMakePresetsTest_NO_BUILD 1)

set(CMakePresetsTest_ASSETS "Good-indexFile.txt")
set(GoodTestPresets
  "minimal;defaults;noEnvironment;withEnvironment"
  "config-debug;config-release"
  "exclude;index;indexFile;showOnly")
run_cmake_test_presets(Good
                       "default"
                       ""
                       "${GoodTestPresets}")
unset(CMakePresetsTest_ASSETS)

set(CMakePresetsTest_NO_CONFIGURE 1)
set(CMakePresetsTest_FILE "${RunCMake_SOURCE_DIR}/Good.json.in")
run_cmake_test_presets(ListPresets "" "" "x" "--list-presets")
unset(CMakePresetsTest_FILE)

run_cmake_test_presets(NoConfigurePreset "" "" "noConfigurePreset")
run_cmake_test_presets(NoTestsAction "default" "" "noTestsAction")
run_cmake_test_presets(Invalid "" "" "hidden;vendorMacro;badConfigurePreset")

set(CMakePresets_SCHEMA_EXPECTED_RESULT 1)
run_cmake_test_presets(PresetsUnsupported "" "" "x")
set(CMakePresets_SCHEMA_EXPECTED_RESULT 0)
set(CMakePresetsTest_NO_CONFIGURE 0)

set(CMakePresetsTest_NO_BUILD 0)
