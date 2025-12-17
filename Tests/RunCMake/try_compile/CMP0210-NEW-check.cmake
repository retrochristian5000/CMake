if (NOT (actual_stdout MATCHES "BADFLAG" OR actual_stderr MATCHES "BADFLAG"))
  set(RunCMake_TEST_FAILED "Not found expected 'BADFLAG'")
endif()
