foreach(lapack_data_models IN LISTS CMake_TEST_FindBLASLAPACK_LPMODES)
  if(lapack_data_models MATCHES "^([^.]+)\\.(.*)$")
    set(lapack_data_model "${CMAKE_MATCH_1}")
    set(lapack_sizeof_integer "${CMAKE_MATCH_2}")
  endif()

  set(CMake_TEST_FindLAPACK            "${CMake_TEST_FindLAPACK_${lapack_data_model}}")
  set(CMake_TEST_FindLAPACK_All        "${CMake_TEST_FindLAPACK_${lapack_data_model}_All}")
  set(CMake_TEST_FindLAPACK_STATIC     "${CMake_TEST_FindLAPACK_${lapack_data_model}_STATIC}")
  set(CMake_TEST_FindLAPACK_STATIC_All "${CMake_TEST_FindLAPACK_${lapack_data_model}_STATIC_All}")

  foreach(variant IN LISTS CMake_TEST_FindLAPACK)
    if(variant MATCHES "^([^.]+)\\.(.*)$")
      set(vendor "${CMAKE_MATCH_1}")
      set(alt_compiler "-DCMAKE_C_COMPILER=${CMAKE_MATCH_2}")
    else()
      set(vendor "${variant}")
      set(alt_compiler "")
    endif()
    if(vendor STREQUAL "All" AND CMake_TEST_FindLAPACK_All)
      set(EXPECT_All "-DEXPECT_All=${CMake_TEST_FindLAPACK_All}")
    else()
      set(EXPECT_All "")
    endif()
    add_test(NAME FindLAPACK.Test_${variant} COMMAND
      ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
      --build-and-test
      "${CMake_SOURCE_DIR}/Tests/FindLAPACK/Test"
      "${CMake_BINARY_DIR}/Tests/FindLAPACK/Test_${variant}_${lapack_data_model}"
      ${build_generator_args}
      --build-project TestFindLAPACK
      --build-options ${build_options} ${alt_compiler} ${EXPECT_All} -DBLA_VENDOR=${vendor} -DBLA_SIZEOF_INTEGER=${lapack_sizeof_integer}
      --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
      )
  endforeach()

  foreach(variant IN LISTS CMake_TEST_FindLAPACK_STATIC)
    if(variant MATCHES "^([^.]+)\\.(.*)$")
      set(vendor "${CMAKE_MATCH_1}")
      set(alt_compiler "-DCMAKE_C_COMPILER=${CMAKE_MATCH_2}")
    else()
      set(vendor "${variant}")
      set(alt_compiler "")
    endif()
    if(vendor STREQUAL "All" AND CMake_TEST_FindLAPACK_STATIC_All)
      set(EXPECT_All "-DEXPECT_All=${CMake_TEST_FindLAPACK_STATIC_All}")
    else()
      set(EXPECT_All "")
    endif()
    add_test(NAME FindLAPACK.Test_${variant}_Static COMMAND
      ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
      --build-and-test
      "${CMake_SOURCE_DIR}/Tests/FindLAPACK/Test"
      "${CMake_BINARY_DIR}/Tests/FindLAPACK/Test_${variant}_${lapack_data_model}_Static"
      ${build_generator_args}
      --build-project TestFindLAPACK
      --build-options ${build_options} ${alt_compiler} ${EXPECT_All} -DBLA_VENDOR=${vendor} -DBLA_SIZEOF_INTEGER=${lapack_sizeof_integer} -DBLA_STATIC=ON
      --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
      )
  endforeach()

endforeach()
