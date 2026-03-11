set(expected
"LibDirs: -L/Test Escaped/Path;-L/SecondPath;-L/Third Path
Includes: -I/Test Escaped/Path;-I/SecondPath;-I/Third Path
"
)

file(READ "${RunCMake_TEST_BINARY_DIR}/escaped-path.txt" actual)

if(NOT(expected STREQUAL actual))
  set(RunCMake_TEST_FAILED "cmake_pkg_config escaped-path.txt does not match expected:\n${actual}")
endif()
