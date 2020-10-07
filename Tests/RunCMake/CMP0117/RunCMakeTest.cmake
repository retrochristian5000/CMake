include(RunCMake)

function(run_cmp0117 status)
  if(NOT status STREQUAL "WARN")
    set(RunCMake_TEST_OPTIONS -DCMAKE_POLICY_DEFAULT_CMP0117=${status})
  endif()
  run_cmake(CMP0117-${status})
endfunction()

run_cmp0117(OLD)
run_cmp0117(WARN)
run_cmp0117(NEW)
