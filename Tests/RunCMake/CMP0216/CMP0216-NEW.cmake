
cmake_policy(SET CMP0216 OLD)

macro(FOO)
endmacro()

get_property(macros DIRECTORY PROPERTY MACROS)
if (macros)
  set(RunCMake_TEST_FAILED "unexpected content for MACROS directory property.")
endif()
