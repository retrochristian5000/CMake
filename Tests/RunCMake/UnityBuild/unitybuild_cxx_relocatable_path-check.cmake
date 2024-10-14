set(unitybuild_cxx "${RunCMake_TEST_BINARY_DIR}/CMakeFiles/tgt.dir/Unity/unity_0_cxx.cxx")
if(NOT EXISTS "${unitybuild_cxx}")
  set(RunCMake_TEST_FAILED "Generated unity source file ${unitybuild_cxx} does not exist.")
  return()
endif()


set(EXPECTED_UNITY_FILE_CONTENT ".*")
foreach(s RANGE 1 3)
  file(RELATIVE_PATH rel_path "${RunCMake_TEST_BINARY_DIR}/CMakeFiles/tgt.dir/Unity" "${RunCMake_TEST_BINARY_DIR}/s${s}.cxx")
  string(APPEND EXPECTED_UNITY_FILE_CONTENT "#include.*\"${rel_path}\".*")
endforeach()

file(STRINGS ${unitybuild_cxx} unitybuild_cxx_strings)
string(REGEX MATCH "${EXPECTED_UNITY_FILE_CONTENT}" matched_code ${unitybuild_cxx_strings})
if(NOT matched_code)
  set(RunCMake_TEST_FAILED "Generated unity file ${unitybuild_cxx} doesn't contain relative paths")
  return()
endif()
