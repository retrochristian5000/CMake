if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()

find_package(Secondary QUIET CONFIG REQUIRED)

if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()
