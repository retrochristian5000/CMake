add_test(NAME FindJNI.Minimal COMMAND
  ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
  --build-and-test
  "${CMake_SOURCE_DIR}/Tests/FindJNI/Minimal"
  "${CMake_BINARY_DIR}/Tests/FindJNI/Minimal"
  ${build_generator_args}
  --build-project TestMinimal
  --build-options ${build_options}
  --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
  )
