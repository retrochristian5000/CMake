string(FIND "${actual_stdout}" "@standard_flag@" standard)
string(FIND "${actual_stdout}" "@extension_flag@" extension)

if(NOT standard EQUAL -1)
  set(RunCMake_TEST_FAILED "Standard compile flag found.")
elseif(NOT extension EQUAL -1)
  set(RunCMake_TEST_FAILED "Extensions compile flag found.")
endif()
