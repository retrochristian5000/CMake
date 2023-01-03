cmake_policy(SET CMP0144 NEW)
set(_findprotobuf_testing TRUE)
find_package(Protobuf MODULE)

if(_findprotobuf_included)
  message(FATAL_ERROR "FindProtobuf.cmake erroneously included")
endif()
