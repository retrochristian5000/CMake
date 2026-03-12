set(expected
"LibDirs: -L${CMAKE_CURRENT_LIST_DIR}/PackageRoot/../../lib;-L${CMAKE_CURRENT_LIST_DIR}/PackageRoot/../../dynlib
Includes: -I${CMAKE_CURRENT_LIST_DIR}/PackageRoot/../../include
"
)

file(READ "${RunCMake_TEST_BINARY_DIR}/pcfiledir.txt" actual)

if(NOT(expected STREQUAL actual))
  set(RunCMake_TEST_FAILED "cmake_pkg_config pcfiledir.txt does not match expected:\n${actual}")
endif()
