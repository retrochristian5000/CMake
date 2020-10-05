add_test(NAME FindCryptoPP.Test COMMAND
  ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
  --build-and-test
  "${CMake_SOURCE_DIR}/Tests/FindCryptoPP/Test"
  "${CMake_BINARY_DIR}/Tests/FindCryptoPP/Test"
  ${build_generator_args}
  --build-project TestFindCryptoPP
  --build-options ${build_options}
  --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
  )
