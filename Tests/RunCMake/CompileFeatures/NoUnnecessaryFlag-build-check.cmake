string(FIND "${actual_stdout}" "@standard_flag@" standard)
string(FIND "${actual_stdout}" "@extension_flag@" extension)

if(NOT (standard EQUAL -1 AND extension EQUAL -1))
  #set(RunCMake_TEST_FAILED "Language version/extensions compile flag found.")
endif()
