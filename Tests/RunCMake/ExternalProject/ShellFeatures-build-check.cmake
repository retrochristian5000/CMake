set(expectedOutput [[
download 1
download 2
update 1
update 2
patch 1
patch 2
configure 1
configure 2
build 1
build genex: UTILITY
install 1
install 2
test 1
test 2
]])

set(outFile ${RunCMake_TEST_BINARY_DIR}/cmdout.txt)
file(READ ${outFile} contents)

if(NOT contents STREQUAL expectedOutput)
  set(RunCMake_TEST_FAILED
    "File contents differ from expected. Actual contents:\n${contents}"
  )
endif()
