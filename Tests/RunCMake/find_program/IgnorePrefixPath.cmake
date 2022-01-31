function(assert_eq var value)
  if(NOT "${${var}}" STREQUAL "${value}")
    message(SEND_ERROR "Expected value of ${var}:\n  ${value}\nActual value:\n  ${${var}}")
  endif()
endfunction()

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Prefix)
set(CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SOURCE_DIR}/SystemPrefix)

set(CMAKE_IGNORE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/SystemPrefix)
set(CMAKE_SYSTEM_IGNORE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/SystemPrefix ${CMAKE_SOURCE_DIR}/Prefix)
find_program(prefixProg
  NAMES prog
  )
assert_eq(prefixProg "${CMAKE_SOURCE_DIR}/Prefix/bin/prog")

set(CMAKE_IGNORE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/SystemPrefix ${CMAKE_SOURCE_DIR}/Prefix)
set(CMAKE_SYSTEM_IGNORE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Prefix)
find_program(systemPrefixProg
  NAMES prog
  )
assert_eq(systemPrefixProg "${CMAKE_SOURCE_DIR}/SystemPrefix/bin/prog")

set(CMAKE_IGNORE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Prefix)
set(CMAKE_SYSTEM_IGNORE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/SystemPrefix)
find_program(noProg
  NAMES prog
  )
assert_eq(noProg "noProg-NOTFOUND")
