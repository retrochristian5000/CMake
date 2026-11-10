cmake_policy(SET CMP0088 OLD)

find_package(BISON REQUIRED)

BISON_TARGET(OldDefault parser.y parser-old-default.cpp)
if(NOT BISON_OldDefault_OUTPUT_HEADER STREQUAL
  "${CMAKE_CURRENT_SOURCE_DIR}/parser-old-default.hpp")
  message(FATAL_ERROR "Unexpected CMP0088 OLD default header: ${BISON_OldDefault_OUTPUT_HEADER}")
endif()

BISON_TARGET(OldExplicit parser.y parser-old.cpp DEFINES_FILE old-custom.h)
if(NOT BISON_OldExplicit_OUTPUT_HEADER STREQUAL "old-custom.h")
  message(FATAL_ERROR "Unexpected CMP0088 OLD Bison header: ${BISON_OldExplicit_OUTPUT_HEADER}")
endif()
