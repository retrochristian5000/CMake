set(blas_data_model lp64)
set(blas_sizeof_integer 4)

set(CMake_TEST_FindBLAS            "${CMake_TEST_FindBLAS_${blas_data_model}}")
set(CMake_TEST_FindBLAS_All        "${CMake_TEST_FindBLAS_${blas_data_model}_All}")
set(CMake_TEST_FindBLAS_STATIC     "${CMake_TEST_FindBLAS_${blas_data_model}_STATIC}")
set(CMake_TEST_FindBLAS_STATIC_All "${CMake_TEST_FindBLAS_${blas_data_model}_STATIC_All}")

foreach(variant IN LISTS CMake_TEST_FindBLAS)
  if(variant MATCHES "^([^.]+)\\.(.*)$")
    set(vendor "${CMAKE_MATCH_1}")
    set(alt_compiler "-DCMAKE_C_COMPILER=${CMAKE_MATCH_2}")
  else()
    set(vendor "${variant}")
    set(alt_compiler "")
  endif()
  if(vendor STREQUAL "All" AND CMake_TEST_FindBLAS_All)
    set(EXPECT_All "-DEXPECT_All=${CMake_TEST_FindBLAS_All}")
  else()
    set(EXPECT_All "")
  endif()
  add_test(NAME FindBLAS.Test_${variant} COMMAND
    ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
    --build-and-test
    "${CMake_SOURCE_DIR}/Tests/FindBLAS/Test"
    "${CMake_BINARY_DIR}/Tests/FindBLAS/Test_${variant}_${blas_data_model}"
    ${build_generator_args}
    --build-project TestFindBLAS
    --build-options ${build_options} ${alt_compiler} ${EXPECT_All} -DBLA_VENDOR=${vendor} -DBLA_SIZEOF_INTEGER=${blas_sizeof_integer}
    --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
    )
endforeach()

foreach(variant IN LISTS CMake_TEST_FindBLAS_STATIC)
  if(variant MATCHES "^([^.]+)\\.(.*)$")
    set(vendor "${CMAKE_MATCH_1}")
    set(alt_compiler "-DCMAKE_C_COMPILER=${CMAKE_MATCH_2}")
  else()
    set(vendor "${variant}")
    set(alt_compiler "")
  endif()
  if(vendor STREQUAL "All" AND CMake_TEST_FindBLAS_STATIC_All)
    set(EXPECT_All "-DEXPECT_All=${CMake_TEST_FindBLAS_STATIC_All}")
  else()
    set(EXPECT_All "")
  endif()
  add_test(NAME FindBLAS.Test_${variant}_Static COMMAND
    ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
    --build-and-test
    "${CMake_SOURCE_DIR}/Tests/FindBLAS/Test"
    "${CMake_BINARY_DIR}/Tests/FindBLAS/Test_${variant}_${blas_data_model}_Static"
    ${build_generator_args}
    --build-project TestFindBLAS
    --build-options ${build_options} ${alt_compiler} ${EXPECT_All} -DBLA_VENDOR=${vendor} -DBLA_SIZEOF_INTEGER=${blas_sizeof_integer} -DBLA_STATIC=ON
    --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
    )
endforeach()
