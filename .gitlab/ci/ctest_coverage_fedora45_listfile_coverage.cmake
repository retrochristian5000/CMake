# Report the CMake code of CMake itself, such as `Modules/`, and leave out
# the CMake code of the tests and of the guides.
list(APPEND CTEST_CUSTOM_COVERAGE_EXCLUDE
  "/Tests/"
  "/Help/guide/"
  )
