
include(RunCMake)

macro(run_cmake_target test subtest target)
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${test}-build)
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(${test}-${subtest} ${CMAKE_COMMAND} --build . --target ${target} --config Release --verbose ${ARGN})

  unset(RunCMake_TEST_BINARY_DIR)
  unset(RunCMake_TEST_NO_CLEAN)
endmacro()

if (RunCMake_GENERATOR MATCHES "Makefiles|Ninja|Visual Studio|Xcode"
    AND NOT CMAKE_C_COMPILER_ID STREQUAL "Intel")

  set(RunCMake_TEST_OUTPUT_MERGE TRUE)
  if (NOT RunCMake_GENERATOR_IS_MULTI_CONFIG)
    set(RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=Release)
  endif()

  run_cmake(LINK_LIBRARY)

  run_cmake_target(LINK_LIBRARY simple LinkLibrary_simple)
  run_cmake_target(LINK_LIBRARY group1 LinkLibrary_group1)
  run_cmake_target(LINK_LIBRARY group2 LinkLibrary_group2)
  run_cmake_target(LINK_LIBRARY nested-feature1 LinkLibrary_nested_feature1)
  run_cmake_target(LINK_LIBRARY nested-feature2 LinkLibrary_nested_feature2)
  run_cmake_target(LINK_LIBRARY link-items1 LinkLibrary_link_items1)
  run_cmake_target(LINK_LIBRARY link-items2 LinkLibrary_link_items2)
  run_cmake_target(LINK_LIBRARY mix-features1 LinkLibrary_mix_features1)
  run_cmake_target(LINK_LIBRARY mix-features2 LinkLibrary_mix_features2)

  unset(RunCMake_TEST_OPTIONS)
  unset(RunCMake_TEST_OUTPUT_MERGE)

endif()
