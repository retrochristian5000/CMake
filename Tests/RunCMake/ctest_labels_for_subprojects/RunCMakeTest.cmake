include(RunCTest)

unset(CTEST_EXTRA_CONFIG)

function(run_ctest_test CASE_NAME)
  run_ctest(${CASE_NAME})
endfunction()

# 1. Specify subprojects via a CTest script variable in the CTest script
function(run_MultipleSubprojects)
  set(CTEST_EXTRA_CONFIG "set(CTEST_USE_LAUNCHERS 1)")
  set(CASE_TEST_PREFIX_CODE [[
file(COPY "${CTEST_RUNCMAKE_SOURCE_DIRECTORY}/MyProductionCode"
  DESTINATION ${CTEST_SOURCE_DIRECTORY})
file(COPY "${CTEST_RUNCMAKE_SOURCE_DIRECTORY}/MyExperimentalFeature"
  DESTINATION ${CTEST_SOURCE_DIRECTORY})

set(CTEST_LABELS_FOR_SUBPROJECTS "MyProductionCode;MyExperimentalFeature")
  ]])
  set(CASE_CMAKELISTS_SUFFIX_CODE [[
add_subdirectory(MyExperimentalFeature)
add_subdirectory(MyProductionCode)
  ]])
  run_ctest_test(MultipleSubprojects)
  unset(CTEST_EXTRA_CONFIG)
endfunction()
run_MultipleSubprojects()

# 2. Specify subprojects via a CTest script variable on the command line e.g.
# ctest -S myscript.cmake -DCTEST_LABELS_FOR_SUBPROJECTS:STRING="MySubproject"
function(run_SubprojectBuildFail)
  set(CTEST_EXTRA_CONFIG "set(CTEST_USE_LAUNCHERS 1)")
  set(CASE_TEST_PREFIX_CODE [[
file(COPY "${CTEST_RUNCMAKE_SOURCE_DIRECTORY}/MyThirdPartyDependency"
  DESTINATION ${CTEST_SOURCE_DIRECTORY})
  ]])
  set(CASE_CMAKELISTS_SUFFIX_CODE [[
add_subdirectory(MyThirdPartyDependency)
  ]])
  run_ctest(SubprojectBuildFail "-DCTEST_LABELS_FOR_SUBPROJECTS:STRING=MyThirdPartyDependency")
  unset(CTEST_EXTRA_CONFIG)
endfunction()
run_SubprojectBuildFail()

# 3. Set subprojects via a CTest module variable in CTestConfig.cmake
function(run_ModuleVariableCTestConfig)
  set(RunCMake_TEST_SOURCE_DIR "${RunCMake_BINARY_DIR}/ModuleVariableCTestConfig")
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/ModuleVariableCTestConfig-build")
  set(RunCMake_TEST_NO_CLEAN 1)
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  set(CTEST_EXTRA_CONFIG "set(CTEST_LABELS_FOR_SUBPROJECTS \"MySubproject\")")
  configure_file(${RunCMake_SOURCE_DIR}/CTestConfig.cmake.in
                 ${RunCMake_TEST_SOURCE_DIR}/CTestConfig.cmake @ONLY)

  set(CASE_CMAKELISTS_SUFFIX_CODE [[
set(someFile "${CMAKE_CURRENT_SOURCE_DIR}/test.cmake")
add_test(NAME SuccessfulTest COMMAND "${CMAKE_COMMAND}" --version)
set_property(TEST SuccessfulTest PROPERTY LABELS MySubproject)
add_test(NAME FailingTest
          COMMAND ${CMAKE_COMMAND} -E compare_files "${someFile}" "${someFile}xxx")
set_property(TEST FailingTest PROPERTY LABELS MySubproject)
add_test(NAME AnotherSuccessfulTest COMMAND "${CMAKE_COMMAND}" --version)
set_property(TEST AnotherSuccessfulTest PROPERTY LABELS NotASubproject)
  ]])
  configure_file(${RunCMake_SOURCE_DIR}/CMakeLists.txt.in
                 ${RunCMake_TEST_SOURCE_DIR}/CMakeLists.txt @ONLY)

  execute_process(
    COMMAND ${CMAKE_COMMAND} ${RunCMake_TEST_SOURCE_DIR}
    WORKING_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  run_cmake_command(ModuleVariableCTestConfig ${CMAKE_CTEST_COMMAND} -C Debug -V)
  unset(CTEST_EXTRA_CONFIG)
endfunction()
run_ModuleVariableCTestConfig()


# 4. Set subprojects via a CTest module variable in CTestConfig.cmake
# Use --no-subproject-summary to disable the Subproject timing summary.
function(run_NoSubprojectSummary)
  set(RunCMake_TEST_SOURCE_DIR "${RunCMake_BINARY_DIR}/NoSubprojectSummary")
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/NoSubprojectSummary-build")
  set(RunCMake_TEST_NO_CLEAN 1)
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  set(CTEST_EXTRA_CONFIG "set(CTEST_LABELS_FOR_SUBPROJECTS \"MySubproject\")")
  configure_file(${RunCMake_SOURCE_DIR}/CTestConfig.cmake.in
                 ${RunCMake_TEST_SOURCE_DIR}/CTestConfig.cmake @ONLY)

  set(CASE_CMAKELISTS_SUFFIX_CODE [[
set(someFile "${CMAKE_CURRENT_SOURCE_DIR}/test.cmake")
add_test(NAME SuccessfulTest COMMAND "${CMAKE_COMMAND}" --version)
set_property(TEST SuccessfulTest PROPERTY LABELS MySubproject)
add_test(NAME FailingTest
          COMMAND ${CMAKE_COMMAND} -E compare_files "${someFile}" "${someFile}xxx")
set_property(TEST FailingTest PROPERTY LABELS MySubproject)
add_test(NAME AnotherSuccessfulTest COMMAND "${CMAKE_COMMAND}" --version)
set_property(TEST AnotherSuccessfulTest PROPERTY LABELS NotASubproject)
  ]])
  configure_file(${RunCMake_SOURCE_DIR}/CMakeLists.txt.in
                 ${RunCMake_TEST_SOURCE_DIR}/CMakeLists.txt @ONLY)

  execute_process(
    COMMAND ${CMAKE_COMMAND} ${RunCMake_TEST_SOURCE_DIR}
    WORKING_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  run_cmake_command(NoSubprojectSummary ${CMAKE_CTEST_COMMAND} -C Debug -V --no-subproject-summary)
  unset(CTEST_EXTRA_CONFIG)
endfunction()
run_NoSubprojectSummary()


# 5. Set subprojects via a CTest module variable on the command line
function(run_ModuleVariableCommandLine)
  set(RunCMake_TEST_SOURCE_DIR "${RunCMake_BINARY_DIR}/ModuleVariableCommandLine")
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/ModuleVariableCommandLine-build")
  set(RunCMake_TEST_NO_CLEAN 1)
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  set(CASE_CMAKELISTS_SUFFIX_CODE [[
set(someFile "${CMAKE_CURRENT_SOURCE_DIR}/test.cmake")
add_test(NAME SuccessfulTest COMMAND "${CMAKE_COMMAND}" --version)
set_property(TEST SuccessfulTest PROPERTY LABELS MySubproject)
add_test(NAME FailingTest
          COMMAND ${CMAKE_COMMAND} -E compare_files "${someFile}" "${someFile}xxx")
set_property(TEST FailingTest PROPERTY LABELS MySubproject)
  ]])
  configure_file(${RunCMake_SOURCE_DIR}/CMakeLists.txt.in
                 ${RunCMake_TEST_SOURCE_DIR}/CMakeLists.txt @ONLY)

  execute_process(
    COMMAND ${CMAKE_COMMAND} ${RunCMake_TEST_SOURCE_DIR} "-DCTEST_LABELS_FOR_SUBPROJECTS:STRING=MySubproject"
    WORKING_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

  run_cmake_command(ModuleVariableCommandLine ${CMAKE_CTEST_COMMAND} -C Debug -V)
endfunction()
run_ModuleVariableCommandLine()

# 6. Set subprojects via a CTest module variable in CTestConfig.cmake.
#    Verify that subprojects are sent to CDash
function(run_ModuleVariableCDash)
  set(CTEST_EXTRA_CONFIG [[
  set(CTEST_USE_LAUNCHERS 1)
  set(CTEST_LABELS_FOR_SUBPROJECTS "MyProductionCode;MyExperimentalFeature")
  ]])
  set(CASE_TEST_PREFIX_CODE [[
file(COPY "${CTEST_RUNCMAKE_SOURCE_DIRECTORY}/MyProductionCode"
  DESTINATION ${CTEST_SOURCE_DIRECTORY})
file(COPY "${CTEST_RUNCMAKE_SOURCE_DIRECTORY}/MyExperimentalFeature"
  DESTINATION ${CTEST_SOURCE_DIRECTORY})
  ]])
  set(CASE_CMAKELISTS_SUFFIX_CODE [[
add_subdirectory(MyExperimentalFeature)
add_subdirectory(MyProductionCode)
  ]])
  run_ctest_test(ModuleVariableCDash)
  unset(CTEST_EXTRA_CONFIG)
endfunction()
run_ModuleVariableCDash()
