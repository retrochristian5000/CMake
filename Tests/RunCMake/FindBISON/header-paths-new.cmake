find_package(BISON REQUIRED)

BISON_TARGET(NewDefault parser.y parser.cpp)
if(NOT BISON_NewDefault_OUTPUT_HEADER STREQUAL
  "${CMAKE_CURRENT_BINARY_DIR}/parser.hpp")
  message(FATAL_ERROR "Unexpected default Bison header: ${BISON_NewDefault_OUTPUT_HEADER}")
endif()
list(FIND BISON_NewDefault_OUTPUTS "${BISON_NewDefault_OUTPUT_HEADER}" _default_header_index)
if(_default_header_index EQUAL -1)
  message(FATAL_ERROR "Default Bison header is missing from BISON_NewDefault_OUTPUTS")
endif()

BISON_TARGET(NewExplicit parser.y parser-explicit.cpp DEFINES_FILE custom.h)
if(NOT BISON_NewExplicit_OUTPUT_HEADER STREQUAL "custom.h")
  message(FATAL_ERROR "Unexpected explicit Bison header: ${BISON_NewExplicit_OUTPUT_HEADER}")
endif()
list(FIND BISON_NewExplicit_OUTPUTS "${BISON_NewExplicit_OUTPUT_HEADER}" _explicit_header_index)
if(_explicit_header_index EQUAL -1)
  message(FATAL_ERROR "Explicit Bison header is missing from BISON_NewExplicit_OUTPUTS")
endif()
list(FIND BISON_NewExplicit_OPTIONS
  "--defines=custom.h" _defines_option_index)
if(_defines_option_index EQUAL -1)
  message(FATAL_ERROR "Bison --defines option does not match its output header")
endif()

BISON_TARGET(NewRelative parser.y parser-relative.cpp
  DEFINES_FILE relative/custom.h)
if(NOT BISON_NewRelative_OUTPUT_HEADER STREQUAL "relative/custom.h")
  message(FATAL_ERROR "Unexpected relative Bison header: ${BISON_NewRelative_OUTPUT_HEADER}")
endif()
list(FIND BISON_NewRelative_OUTPUTS "${BISON_NewRelative_OUTPUT_HEADER}" _relative_header_index)
if(_relative_header_index EQUAL -1)
  message(FATAL_ERROR "Relative Bison header is missing from BISON_NewRelative_OUTPUTS")
endif()
list(FIND BISON_NewRelative_OPTIONS
  "--defines=relative/custom.h" _relative_defines_option_index)
if(_relative_defines_option_index EQUAL -1)
  message(FATAL_ERROR "Relative Bison --defines option does not match its output header")
endif()

BISON_TARGET(NewAbsolute parser.y parser-absolute.cpp
  DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/absolute.h")
if(NOT BISON_NewAbsolute_OUTPUT_HEADER STREQUAL
    "${CMAKE_CURRENT_BINARY_DIR}/absolute.h")
  message(FATAL_ERROR "Unexpected absolute Bison header: ${BISON_NewAbsolute_OUTPUT_HEADER}")
endif()
list(FIND BISON_NewAbsolute_OUTPUTS "${BISON_NewAbsolute_OUTPUT_HEADER}" _absolute_header_index)
if(_absolute_header_index EQUAL -1)
  message(FATAL_ERROR "Absolute Bison header is missing from BISON_NewAbsolute_OUTPUTS")
endif()
list(FIND BISON_NewAbsolute_OPTIONS
  "--defines=${BISON_NewAbsolute_OUTPUT_HEADER}" _absolute_defines_option_index)
if(_absolute_defines_option_index EQUAL -1)
  message(FATAL_ERROR "Absolute Bison --defines option does not match its output header")
endif()

BISON_TARGET(NewSourceAbsolute parser.y parser-source-absolute.cpp
  DEFINES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/source-absolute.h")
if(NOT BISON_NewSourceAbsolute_OUTPUT_HEADER STREQUAL
    "${CMAKE_CURRENT_SOURCE_DIR}/source-absolute.h")
  message(FATAL_ERROR
    "Unexpected source-absolute Bison header: ${BISON_NewSourceAbsolute_OUTPUT_HEADER}")
endif()
list(FIND BISON_NewSourceAbsolute_OUTPUTS
  "${BISON_NewSourceAbsolute_OUTPUT_HEADER}" _source_absolute_header_index)
if(_source_absolute_header_index EQUAL -1)
  message(FATAL_ERROR "Source-absolute Bison header is missing from BISON_NewSourceAbsolute_OUTPUTS")
endif()
list(FIND BISON_NewSourceAbsolute_OPTIONS
  "--defines=${BISON_NewSourceAbsolute_OUTPUT_HEADER}"
  _source_absolute_defines_option_index)
if(_source_absolute_defines_option_index EQUAL -1)
  message(FATAL_ERROR
    "Source-absolute Bison --defines option does not match its output header")
endif()

add_custom_target(generated_headers ALL
  DEPENDS
    ${BISON_NewDefault_OUTPUTS}
    ${BISON_NewExplicit_OUTPUTS}
    ${BISON_NewRelative_OUTPUTS}
    ${BISON_NewAbsolute_OUTPUTS}
  )
