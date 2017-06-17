cmake_minimum_required(VERSION 3.8)
project(CSharpCustomCommand CSharp)

add_executable(CSharpCustomCommand dummy.cs)

set(generatedFileName "${CMAKE_CURRENT_BINARY_DIR}/test.cs")

message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")

if(EXISTS ${generatedFileName})
  message(STATUS "removing generated file")
  file(REMOVE ${generatedFileName})
endif()

add_custom_command(OUTPUT ${generatedFileName}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cs.in
    ${generatedFileName}
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/test.cs.in
  COMMENT "Generating test.cs")

target_sources(CSharpCustomCommand PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cs.in
  ${generatedFileName})
