string(FIND "${actual_stdout}" "@flag@" flag)

if(flag EQUAL -1)
  set(RunCMake_TEST_FAILED "Compile flag \"@flag@\" for CMAKE_@lang@_EXTENSIONS=@extensions@ not found.")
endif()
