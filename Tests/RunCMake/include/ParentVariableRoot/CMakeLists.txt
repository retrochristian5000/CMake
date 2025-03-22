cmake_minimum_required(VERSION 4.0)
project(ParentVariableDir)

if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()

include("include1.cmake")

if(NOT "${CMAKE_PARENT_LIST_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
  message(SEND_ERROR "Bad value of `CMAKE_PARENT_LIST_FILE`")
endif()
