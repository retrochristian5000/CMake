# The executable has the property set, so it is linked with its own profile
# data directory, as in the USE phase, and not with the one of the library.
if(CMAKE_C_PGO_MERGE_OUTPUT MATCHES "\.pgd$")
  # The library's profile data directory is not used, as the profile data
  # is written next to the executable, and merged into its '.pgd' file.
  file(GLOB_RECURSE pgd "${RunCMake_TEST_BINARY_DIR}/pgo/*main.pgd")
  if(NOT pgd)
    set(RunCMake_TEST_FAILED "main.pgd not found in the profile data directory of 'main'.")
  endif()
elseif(NOT IS_DIRECTORY "${RunCMake_TEST_BINARY_DIR}/pgo-foo")
  set(RunCMake_TEST_FAILED "Profile data directory of 'foo' not found.")
endif()
