cmake_minimum_required(VERSION 3.18)

project(PchReuseFromObjLib)

set(CMAKE_PCH_WARN_INVALID OFF)

if(CMAKE_CXX_COMPILE_OPTIONS_USE_PCH)
  add_definitions(-DHAVE_PCH_SUPPORT)
endif()

######################################################################

file(WRITE ${CMAKE_BINARY_DIR}/pch.cxx [=[
void nothing()
{
}
]=])

file(WRITE ${CMAKE_BINARY_DIR}/string.hxx [=[
#include <string.h>

namespace std {
  struct string
  {
    char storage[20];

    string(const char* s) {
      strcpy(storage, s);
    }

    const char* c_str() const {
      return storage;
    }
  };
}
]=])

add_library(pch-generator ${pch-generator-type} ${CMAKE_BINARY_DIR}/pch.cxx)
set_property(TARGET pch-generator PROPERTY POSITION_INDEPENDENT_CODE ON)
target_precompile_headers(pch-generator PRIVATE ${CMAKE_BINARY_DIR}/string.hxx)

######################################################################

file(WRITE ${CMAKE_BINARY_DIR}/message.cxx [=[
#include "message.hxx"

#ifndef HAVE_PCH_SUPPORT
  #include "string.hxx"
#endif

const char* message()
{
  static std::string greeting("hi there");
  return greeting.c_str();
}
]=])

file(WRITE ${CMAKE_BINARY_DIR}/message.hxx [=[
#ifdef WIN32_BUILD_SHARED
  #ifdef BUILD_LIBRARY
    #define MESSAGE_EXPORT __declspec(dllexport)
  #else
    #define MESSAGE_EXPORT __declspec(dllimport)
  #endif
#else
  #define MESSAGE_EXPORT
#endif

MESSAGE_EXPORT const char* message();
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

  set_property(TARGET message_${type} PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_property(TARGET message_${type} PROPERTY DEFINE_SYMBOL "")

  if (WIN32 AND type STREQUAL "SHARED")
    target_compile_definitions(message_${type} PRIVATE BUILD_LIBRARY)
    target_compile_definitions(message_${type} PUBLIC WIN32_BUILD_SHARED)
  endif()

  add_executable(main_${type} ${CMAKE_BINARY_DIR}/main.cxx)
  target_include_directories(main_${type} PRIVATE ${CMAKE_BINARY_DIR})

  target_link_libraries(main_${type} PRIVATE message_${type})

  add_test(NAME main_${type} COMMAND main_${type})
endfunction()

foreach(type OBJECT STATIC SHARED)
  add_library_and_executable(${type})
endforeach()
