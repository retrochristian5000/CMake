if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" MATCHES "^/")
  set(slash /)
endif()
file(DOWNLOAD
  "file://${slash}${CMAKE_CURRENT_SOURCE_DIR}/DOWNLOAD-hash-mismatch.txt"
  ${CMAKE_CURRENT_BINARY_DIR}/hash-mismatch.txt
  STATUS status
  HASH_MISMATCH_ERROR_VARIABLE hash_mismatch
  )
