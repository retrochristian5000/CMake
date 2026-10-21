if(NOT RunCMake_GENERATOR MATCHES "Ninja|Makefiles")
  # Other generators do not print compile lines that can be scanned.
  return()
endif()

include("${RunCMake_TEST_BINARY_DIR}/expected-flags.cmake")

if(expect_pic_flags STREQUAL "")
  # The compiler has no position independent code flags, e.g. MSVC.
  return()
endif()

# Check the flags on every compile line that produces an object of the given
# target.  The flags of the other flavor must not appear, unless both flavors
# use the same flags.
macro(check_target_flags target expected unexpected)
  string(REGEX MATCHALL "[^\n]* -o [^ \n]*CMakeFiles/${target}\\.dir/[^\n]*"
    compile_lines "${actual_stdout}")
  if(NOT compile_lines)
    list(APPEND RunCMake_TEST_FAILED
      "No compile line found for target '${target}'")
  endif()
  foreach(compile_line IN LISTS compile_lines)
    if(NOT "${expected}" STREQUAL "")
      string(FIND " ${compile_line} " " ${expected} " found)
      if(found EQUAL -1)
        list(APPEND RunCMake_TEST_FAILED
          "Target '${target}' is missing the '${expected}' flags:\n ${compile_line}")
      endif()
    endif()
    if(NOT "${unexpected}" STREQUAL "" AND
       NOT "${unexpected}" STREQUAL "${expected}")
      string(FIND " ${compile_line} " " ${unexpected} " found)
      if(NOT found EQUAL -1)
        list(APPEND RunCMake_TEST_FAILED
          "Target '${target}' has the '${unexpected}' flags:\n ${compile_line}")
      endif()
    endif()
  endforeach()
endmacro()

foreach(target IN LISTS expect_pic_targets)
  check_target_flags("${target}" "${expect_pic_flags}" "${expect_pie_flags}")
endforeach()

foreach(target IN LISTS expect_pie_targets)
  check_target_flags("${target}" "${expect_pie_flags}" "${expect_pic_flags}")
endforeach()

foreach(target IN LISTS expect_nopic_targets)
  check_target_flags("${target}" "" "${expect_pic_flags}")
  check_target_flags("${target}" "" "${expect_pie_flags}")
endforeach()
