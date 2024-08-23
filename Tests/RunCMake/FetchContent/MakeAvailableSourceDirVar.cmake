include(FetchContent)

# Enforce that CMP0126 is old in the caller
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Lowercase is critical since FetchContent sets
#  ``<lowercaseName>_SOURCE_DIR`
FetchContent_Declare(
  cmp0126
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/cmp0126
  SOURCE_SUBDIR cpp
)
FetchContent_MakeAvailable(cmp0126)

get_property(added_project_cmp0126 GLOBAL PROPERTY project_cmp0126 SET)
if(NOT added_project_cmp0126)
  message(SEND_ERROR "cmp0126/cpp CMakeLists.txt not added")
endif()
