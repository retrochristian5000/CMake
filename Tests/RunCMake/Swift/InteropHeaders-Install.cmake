cmake_policy(SET CMP0157 NEW)

enable_language(Swift)

add_library(L interop-lib.swift)
target_sources(L PUBLIC
  GENERATE_INTEROP_HEADER Swift gen/swift/answer.h FILE_SET HEADERS BASE_DIR gen
)

install(TARGETS L
  ARCHIVE DESTINATION lib
  FILE_SET HEADERS DESTINATION include
)
