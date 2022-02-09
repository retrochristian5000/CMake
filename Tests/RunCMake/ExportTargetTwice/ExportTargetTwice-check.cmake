# This file does a regex file compare on the generated
# Android.mk files from the ExportTargetTwice test

macro(compare_file_to_expected file expected_file)
  file(READ "${file}" actual)
  # clean up new lines
  string(REGEX REPLACE "\r\n" "\n" actual "${actual}")
  string(REGEX REPLACE "\n+$" "" actual "${actual}")
  # read in the expected regex file
  file(READ "${expected_file}" expected)
  # clean up new lines
  string(REGEX REPLACE "\n+$" "" expected "${expected}")
  # compare the file to the expected regex and if there is not a match
  # put an error message in RunCMake_TEST_FAILED
  if(NOT "${actual}" MATCHES "${expected}")
    set(RunCMake_TEST_FAILED
      "${file} does not match ${expected_file}:

Actual = [\n${actual}\n]
Expected = [\n${expected}\n]")
  endif()
endmacro()

compare_file_to_expected(
"${RunCMake_BINARY_DIR}/ExportTargetTwice-build/CMakeFiles/Export/pkg1/pkg1Config.cmake"
"${RunCMake_TEST_SOURCE_DIR}/expected-pkg1Config.txt")
compare_file_to_expected(
"${RunCMake_BINARY_DIR}/ExportTargetTwice-build/CMakeFiles/Export/pkg2/pkg2Config.cmake"
"${RunCMake_TEST_SOURCE_DIR}/expected-pkg2Config.txt")
