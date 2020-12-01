cmake_policy(SET CMP0119 NEW)

enable_language(CXX)
add_executable(AltExtCXX AltExtCXX.zzz)
set_property(SOURCE AltExtCXX.zzz PROPERTY LANGUAGE CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "XL")
  # Even with -qsourcetype=c, xlc complains about an unknown suffix:
  #   1501-218 (W) file /.../AltExtC.zzz contains an incorrect file suffix
  # It returns non-zero even with '-qsuppress=1501-218'.
  # Skip the C part of the test.
  return()
endif()

enable_language(C)
add_executable(AltExtC AltExtC.zzz)
set_property(SOURCE AltExtC.zzz PROPERTY LANGUAGE C)
