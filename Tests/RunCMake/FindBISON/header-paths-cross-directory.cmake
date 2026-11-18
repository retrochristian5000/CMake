find_package(BISON REQUIRED)

cmake_policy(SET CMP0088 NEW)

BISON_TARGET(Parser
  "${CMAKE_CURRENT_SOURCE_DIR}/bison/parser.y"
  bison/parser.cpp)

if(NOT BISON_Parser_OUTPUT_HEADER STREQUAL
    "${CMAKE_CURRENT_BINARY_DIR}/bison/parser.hpp")
  message(FATAL_ERROR
    "Unexpected cross-directory Bison header: ${BISON_Parser_OUTPUT_HEADER}")
endif()

add_executable(parser_demo
  ${BISON_Parser_OUTPUTS}
  main.cpp)
target_include_directories(parser_demo PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/bison")
