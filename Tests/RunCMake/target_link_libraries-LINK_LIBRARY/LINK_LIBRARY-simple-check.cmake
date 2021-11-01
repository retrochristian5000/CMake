
if (NOT actual_stdout MATCHES "[/-]-LIBFLAG.*(lib)?base1")
  set (RunCMake_TEST_FAILED "Not found expected '--LIBFLAG<base1>'.")
endif()
