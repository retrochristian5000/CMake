if(DEFINED CMAKE_PARENT_LIST_FILE)
  message(SEND_ERROR "`CMAKE_PARENT_LIST_FILE` is not expected to be set here")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/include1.cmake")

if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/Script.cmake")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()
