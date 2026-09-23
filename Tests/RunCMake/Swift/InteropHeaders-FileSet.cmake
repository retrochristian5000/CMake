cmake_policy(SET CMP0157 NEW)

enable_language(Swift)

add_library(L interop-lib.swift)

target_sources(L PUBLIC
  GENERATE_INTEROP_HEADER Swift generated/swift/answer.h FILE_SET HEADERS BASE_DIR generated)

get_target_property(dirs L HEADER_DIRS)
get_target_property(headers L HEADER_SET)

message(STATUS "HEADER_DIR=[${dirs}]")
message(STATUS "HEADER_SET=[${headers}]")
