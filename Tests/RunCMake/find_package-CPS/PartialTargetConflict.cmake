cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)

add_library(ScopeTest::target INTERFACE IMPORTED)
set_property(TARGET ScopeTest::target PROPERTY
  INTERFACE_COMPILE_DEFINITIONS EXISTING)
find_package(ScopeTest CONFIG REQUIRED COMPONENTS target)

foreach(target IN ITEMS
    ScopeTest::symbolic ScopeTest::tool ScopeTest ScopeTest::extra)
  if(TARGET ${target})
    message(FATAL_ERROR "${target} was created despite the target conflict !")
  endif()
endforeach()
get_target_property(definitions ScopeTest::target INTERFACE_COMPILE_DEFINITIONS)
if(NOT definitions STREQUAL "EXISTING")
  message(FATAL_ERROR
    "ScopeTest::target has wrong definitions '${definitions}' !")
endif()
