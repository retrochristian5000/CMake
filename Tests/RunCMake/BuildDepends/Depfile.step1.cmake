file(REMOVE ${RunCMake_TEST_BINARY_DIR}/ccstamp.txt)
file(REMOVE ${RunCMake_TEST_BINARY_DIR}/exestamp.txt)
file(REMOVE ${RunCMake_TEST_BINARY_DIR}/libstamp.txt)

file(TOUCH ${RunCMake_TEST_BINARY_DIR}/ccdep.txt)
file(TOUCH ${RunCMake_TEST_BINARY_DIR}/exedep.txt)
file(TOUCH ${RunCMake_TEST_BINARY_DIR}/libdep.txt)
