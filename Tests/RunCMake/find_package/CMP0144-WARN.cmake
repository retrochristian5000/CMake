set(_findprotobuf_testing TRUE)
find_package(Protobuf MODULE)

if(NOT _findprotobuf_included)
  message(FATAL_ERROR "FindProtobuf.cmake not included")
endif()
