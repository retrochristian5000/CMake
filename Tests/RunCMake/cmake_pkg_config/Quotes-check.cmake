set(expected
[=[LibDirs: -L/Test Quoted/Path;-L/SecondPath";-L/Third /Path
Includes: -I/Test Quoted/Path;-I/SecondPath";-I/Third /Path
]=]
)

file(READ "${RunCMake_TEST_BINARY_DIR}/quotes.txt" actual)

if(NOT(expected STREQUAL actual))
  set(RunCMake_TEST_FAILED "cmake_pkg_config quotes.txt does not match expected:\n${actual}")
endif()
