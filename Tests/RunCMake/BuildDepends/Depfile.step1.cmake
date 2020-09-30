file(REMOVE ${RunCMake_TEST_BINARY_DIR}/Depfile/ccstamp.txt)
file(REMOVE ${RunCMake_TEST_BINARY_DIR}/Depfile/exestamp.txt)
file(REMOVE ${RunCMake_TEST_BINARY_DIR}/Depfile/libstamp.txt)

file(MAKE_DIRECTORY ${RunCMake_TEST_BINARY_DIR}/Depfile)
file(TOUCH ${RunCMake_TEST_BINARY_DIR}/Depfile/ccdep.txt)
file(TOUCH ${RunCMake_TEST_BINARY_DIR}/Depfile/exedep.txt)
file(TOUCH ${RunCMake_TEST_BINARY_DIR}/Depfile/libdep.txt)
