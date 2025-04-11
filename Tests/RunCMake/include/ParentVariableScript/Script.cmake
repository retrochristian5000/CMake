if(DEFINED CMAKE_PARENT_LIST_FILE)
  message(SEND_ERROR "`CMAKE_PARENT_LIST_FILE` is not expected to be set here")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/include1.cmake")

if(DEFINED CMAKE_PARENT_LIST_FILE)
  message(SEND_ERROR "`CMAKE_PARENT_LIST_FILE` is not expected to be set here")
endif()
