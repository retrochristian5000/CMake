set(expectedOutput [[
download 1
download 2: !@#$%^& *()\\\"':[]{}|`~?<>,.
update 1
update 2
patch 1
patch 2
]])

set(outFile ${RunCMake_TEST_BINARY_DIR}/cmdout.txt)
file(READ ${outFile} contents)

if(NOT contents STREQUAL expectedOutput)
  set(RunCMake_TEST_FAILED
    "File contents differ from expected. Actual contents:\n${contents}"
  )
endif()
