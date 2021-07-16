function(assert_prop_eq tgt prop value)
  unset(actual_value)
  get_property(actual_value TARGET ${tgt} PROPERTY ${prop})
  if(NOT actual_value STREQUAL value)
    message(SEND_ERROR "Expected value of ${prop}:\n  ${value}\nActual value:\n  ${actual_value}")
  endif()
endfunction()

function(assert_prop_matches tgt prop regex)
  unset(actual_value)
  get_property(actual_value TARGET ${tgt} PROPERTY ${prop})
  if(NOT actual_value MATCHES "${regex}")
    message(SEND_ERROR "Expected value of ${prop}:\n  ${regex}\nActual value:\n  ${actual_value}")
  endif()
endfunction()

include("${CMAKE_BINARY_DIR}/../HeadersExport-build/export.cmake")
include("${CMAKE_BINARY_DIR}/../HeadersExport-build/install/lib/cmake/export.cmake")

assert_prop_eq(export::lib1 HEADER_SETS "")
assert_prop_eq(export::lib1 INTERFACE_HEADER_SETS "b;c")
assert_prop_eq(export::lib1 HEADER_SET "error.c")
assert_prop_matches(export::lib1 HEADER_DIR "/Tests/RunCMake/target_headers$")
assert_prop_eq(export::lib1 HEADER_SET_b "h2.h")
assert_prop_matches(export::lib1 HEADER_DIR_b "/Tests/RunCMake/target_headers$")
assert_prop_eq(export::lib1 HEADER_SET_c "$<1:dir.h>")
assert_prop_matches(export::lib1 HEADER_DIR_c "^\\$<1:[^;]*/Tests/RunCMake/target_headers/dir>$")
assert_prop_matches(export::lib1 INTERFACE_INCLUDE_DIRECTORIES "^[^;]*/Tests/RunCMake/target_headers;[^;]*/Tests/RunCMake/target_headers;\\$<1:[^;]*/Tests/RunCMake/target_headers/dir>;\\$<BUILD_INTERFACE:[^;]*/Tests/RunCMake/target_headers>;\\$<BUILD_INTERFACE:[^;]*/Tests/RunCMake/target_headers>;\\$<BUILD_INTERFACE:\\$<1:[^;]*/Tests/RunCMake/target_headers/dir>>$")

assert_prop_eq(install::lib1 HEADER_SETS "")
assert_prop_eq(install::lib1 INTERFACE_HEADER_SETS "b;c")
assert_prop_eq(install::lib1 HEADER_SET "error.c")
assert_prop_matches(install::lib1 HEADER_DIR "/Tests/RunCMake/target_headers/HeadersExport-build/install/include$")
assert_prop_eq(install::lib1 HEADER_SET_b "h2.h")
assert_prop_matches(install::lib1 HEADER_DIR_b "/Tests/RunCMake/target_headers/HeadersExport-build/install/include$")
assert_prop_eq(install::lib1 HEADER_SET_c "$<1:dir.h>")
assert_prop_matches(install::lib1 HEADER_DIR_c "/Tests/RunCMake/target_headers/HeadersExport-build/install/include/dir$")
assert_prop_matches(install::lib1 INTERFACE_INCLUDE_DIRECTORIES "^\\$<BUILD_INTERFACE:[^;]*/Tests/RunCMake/target_headers/HeadersExport-build/install/include>;\\$<BUILD_INTERFACE:[^;]*/Tests/RunCMake/target_headers/HeadersExport-build/install/include>;\\$<BUILD_INTERFACE:[^;]*/Tests/RunCMake/target_headers/HeadersExport-build/install/include/dir>$")

file(GLOB_RECURSE actual
  LIST_DIRECTORIES TRUE
  RELATIVE "${CMAKE_BINARY_DIR}/../HeadersExport-build/install/include"
  "${CMAKE_BINARY_DIR}/../HeadersExport-build/install/include/*"
  )
if(actual)
  list(SORT actual)
endif()
if(NOT "${actual}" MATCHES "^dir;dir/dir\.h;error\.c;h2\.h")
  message(SEND_ERROR "Installed files:
  ${actual}
do not match what we expected:
  ${expect}
in directory:
  ${CMAKE_INSTALL_PREFIX}")
endif()
