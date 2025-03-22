if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()

include("Inc/include2.cmake")

if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/include1.cmake")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()

include("Inc/CMakeLists.txt")

if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/include1.cmake")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()
