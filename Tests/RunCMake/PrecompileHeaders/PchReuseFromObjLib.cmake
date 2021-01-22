cmake_minimum_required(VERSION 3.18)

project(PchReuseFromObjLib)

######################################################################

file(WRITE ${CMAKE_BINARY_DIR}/pch.cxx [=[
void nothing()
{
}
]=])

add_library(pch-generator OBJECT ${CMAKE_BINARY_DIR}/pch.cxx)
target_precompile_headers(pch-generator PRIVATE <string>)

######################################################################

file(WRITE ${CMAKE_BINARY_DIR}/message.cxx [=[
#include "message.hxx"
const char* message()
{
  static std::string greeting("hi there");
  return greeting.c_str();
}
]=])

file(WRITE ${CMAKE_BINARY_DIR}/message.hxx [=[
extern "C" const char* message();
]=])

file(WRITE ${CMAKE_BINARY_DIR}/message.def [=[
EXPORTS
  message
]=])

######################################################################

file(WRITE ${CMAKE_BINARY_DIR}/main.cxx [=[
#include "message.hxx"
#include <string.h>

int main()
{
  return strcmp(message(), "hi there");
}
]=])

######################################################################

enable_testing()

function(add_library_and_executable type)
  add_library(message_${type} ${type} ${CMAKE_BINARY_DIR}/message.cxx)
  target_precompile_headers(message_${type} REUSE_FROM pch-generator)

  if (WIN32 AND type STREQUAL "SHARED")
    target_sources(message_${type} PRIVATE ${CMAKE_BINARY_DIR}/message.def)
  endif()

  add_executable(main_${type} ${CMAKE_BINARY_DIR}/main.cxx)
  target_include_directories(main_${type} PRIVATE ${CMAKE_BINARY_DIR})

  target_link_libraries(main_${type} PRIVATE message_${type})

  add_test(NAME main_${type} COMMAND main_${type})
endfunction()

foreach(type OBJECT STATIC SHARED)
  add_library_and_executable(${type})
endforeach()
