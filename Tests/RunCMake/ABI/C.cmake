enable_language(C)
if(NOT CMAKE_C_BYTE_ORDER MATCHES "^(BIG_ENDIAN|LITTLE_ENDIAN)$")
  message(FATAL_ERROR "CMAKE_C_BYTE_ORDER has unexpected value '${CMAKE_C_BYTE_ORDER}'")
endif()
