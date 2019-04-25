
cmake_minimum_required(VERSION 3.14)

enable_language (C)

set (win_platforms Windows CYGWIN)

set (GENERATE_CONTENT [[
macro (CHECK_VALUE test_msg value expected)
  if (NOT "${value}" STREQUAL "${expected}")
    string (APPEND RunCMake_TEST_FAILED "${test_msg}: actual result:\n [${value}]\nbut expected:\n [${expected}]\n")
  endif()
endmacro()
]])

add_executable(exec1 IMPORTED)
add_library (shared1 SHARED IMPORTED)
add_library (static1 STATIC IMPORTED)

string (APPEND GENERATE_CONTENT
"\ncheck_value (\"TARGET_FILE_POSTFIX executable default\" \"$<TARGET_FILE_POSTFIX:exec1>\" \"\")
check_value (\"TARGET_FILE_POSTFIX shared default\" \"$<TARGET_FILE_POSTFIX:shared1>\" \"\")
check_value (\"TARGET_FILE_POSTFIX static default\" \"$<TARGET_FILE_POSTFIX:static1>\" \"\")")


add_executable (exec2 IMPORTED)
add_library (shared2 SHARED IMPORTED)
add_library (static2 STATIC IMPORTED)

get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_isMultiConfig)
  list(GET CMAKE_CONFIGURATION_TYPES 0 config)
  set(GENERATE_CONDITION CONDITION $<CONFIG:${config}>)
else()
  set (config "${CMAKE_BUILD_TYPE}")
endif()
string (TOUPPER "${config}" config)
set_property (TARGET exec2 PROPERTY ${config}_POSTFIX exec2_postfix)
set_property (TARGET shared2 PROPERTY ${config}_POSTFIX shared2_postfix)
set_property (TARGET static2 PROPERTY ${config}_POSTFIX static2_postfix)

string (APPEND GENERATE_CONTENT
"\ncheck_value (\"TARGET_FILE_PREFIX executable custom\" \"$<TARGET_FILE_POSTFIX:exec2>\" \"exec2_postfix\")
check_value (\"TARGET_FILE_POSTFIX shared custom\" \"$<TARGET_FILE_POSTFIX:shared2>\" \"shared2_postfix\")
check_value (\"TARGET_FILE_POSTFIX static custom\" \"$<TARGET_FILE_POSTFIX:static2>\" \"static2_postfix\")")


file (GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/TARGET_FILE_POSTFIX-generated.cmake"
  CONTENT "${GENERATE_CONTENT}" ${GENERATE_CONDITION})
