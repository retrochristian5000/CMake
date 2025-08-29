string(REPLACE "&" ";" CPACK_PACKAGE_CHECKSUM "${RunCMake_SUBTEST_SUFFIX}")

file(GLOB PACKAGE RELATIVE ${bin_dir} "*.tar.gz")

foreach(_suffix IN LISTS CPACK_PACKAGE_CHECKSUM)
  string(TOLOWER ${_suffix} CHECKSUM_EXTENSION)
  file(STRINGS ${PACKAGE}.${CHECKSUM_EXTENSION} CHSUM_VALUE)
  file(${_suffix} ${PACKAGE} expected_value )
  set(expected_value "${expected_value}  ${PACKAGE}")

  if(NOT expected_value STREQUAL CHSUM_VALUE)
    message(FATAL_ERROR "Generated checksum is not valid! Expected [${expected_value}] Got [${CHSUM_VALUE}]")
  endif()
endforeach()
