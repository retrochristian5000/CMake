include(RunCMake)

function(run_cmake_build_presets name CMakePresetsBuild_CONFIGURE_PRESETS CMakePresetsBuild_BUILD_PRESETS)
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

  if(NOT CMakePresetsBuild_FILE)
    set(CMakePresetsBuild_FILE "${RunCMake_SOURCE_DIR}/${name}.json.in")
  endif()
  if(EXISTS "${CMakePresetsBuild_FILE}")
    configure_file("${CMakePresetsBuild_FILE}" "${RunCMake_TEST_SOURCE_DIR}/CMakePresets.json" @ONLY)
  endif()

  if(NOT CMakeUserPresets_FILE)
    set(CMakeUserPresets_FILE "${RunCMake_SOURCE_DIR}/${name}User.json.in")
  endif()
  if(EXISTS "${CMakeUserPresets_FILE}")
    configure_file("${CMakeUserPresets_FILE}" "${RunCMake_TEST_SOURCE_DIR}/CMakeUserPresets.json" @ONLY)
  endif()

  if (NOT CMakePresetsBuild_BUILD_ONLY)
    foreach(CONFIGURE_PRESET ${CMakePresetsBuild_CONFIGURE_PRESETS})
      set(CMakePresetsBuild_CONFIGURE_OUTPUT_FILE
        "${RunCMake_TEST_COMMAND_WORKING_DIRECTORY}/configure-${name}-${CONFIGURE_PRESET}.txt")

      execute_process(COMMAND "${CMAKE_COMMAND}"
        "--preset" "${CONFIGURE_PRESET}"
        WORKING_DIRECTORY "${RunCMake_TEST_COMMAND_WORKING_DIRECTORY}"
        RESULT_VARIABLE RESULT
        OUTPUT_FILE "${CMakePresetsBuild_CONFIGURE_OUTPUT_FILE}"
        ERROR_FILE "${CMakePresetsBuild_CONFIGURE_OUTPUT_FILE}")
      if(RESULT)
        message(FATAL_ERROR "Failed to configure project for test '${name}' (see ${CMakePresetsBuild_CONFIGURE_OUTPUT_FILE})")
      endif()
    endforeach()
  endif()

  foreach(BUILD_PRESET ${CMakePresetsBuild_BUILD_PRESETS})
    if (EXISTS "${RunCMake_SOURCE_DIR}/${name}-${BUILD_PRESET}-check.cmake")
      set(RunCMake-check-file "${name}-${BUILD_PRESET}-check.cmake")
    else()
      set(RunCMake-check-file "check.cmake")
    endif()
    set(RunCMake-stdout-file "${name}-${BUILD_PRESET}-stdout.txt")
    set(RunCMake-stderr-file "${name}-${BUILD_PRESET}-stderr.txt")

    set(RunCMake_TEST_COMMAND ${CMAKE_COMMAND}
      "--build"
      "--preset" "${BUILD_PRESET}"
      ${ARGN})

    run_cmake(${name})
  endforeach()
endfunction()

set(CMakePresets_SCHEMA_EXPECTED_RESULT 0)

configure_file("${RunCMake_SOURCE_DIR}/Good-macros-stdout.txt.in" "${RunCMake_SOURCE_DIR}/Good-macros-stdout.txt" @ONLY)
run_cmake_build_presets(Good "default;other" "build-other;withEnvironment;noEnvironment;macros;vendorObject")
file(REMOVE "${RunCMake_SOURCE_DIR}/Good-macros-stdout.txt")

set(CMakePresetsBuild_BUILD_ONLY 1)
run_cmake_build_presets(ListPresets "x" "x" "--list-presets")
run_cmake_build_presets(NoConfigurePreset "x" "noConfigurePreset")
run_cmake_build_presets(Invalid "x" "hidden;vendorMacro;badConfigurePreset")

set(CMakePresets_SCHEMA_EXPECTED_RESULT 1)
run_cmake_build_presets(PresetsUnsupported "x" "x")
set(CMakePresets_SCHEMA_EXPECTED_RESULT 0)
set(CMakePresetsBuild_BUILD_ONLY 0)
