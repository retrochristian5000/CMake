include("${CMAKE_CURRENT_LIST_DIR}/Workflow-merge-check.cmake")
if(RunCMake_TEST_FAILED OR NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  return()
endif()
# The merged profile data accumulates the profile data of both training
# runs, each running the loop of 'foo' 100000 times.
list(GET CMAKE_C_PGO_MERGE 0 profdata)
execute_process(COMMAND "${profdata}" show "${merged}"
  OUTPUT_VARIABLE show_out
  RESULT_VARIABLE show_res)
if(NOT show_res EQUAL 0 OR NOT show_out MATCHES "Maximum function count: 200000\n")
  set(RunCMake_TEST_FAILED
    "Merged profile data does not hold both training runs:\n${show_out}")
endif()
