cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)

add_library(ScopeTest INTERFACE IMPORTED)
set_property(TARGET ScopeTest PROPERTY INTERFACE_LINK_LIBRARIES EXISTING)
find_package(ScopeTest CONFIG REQUIRED COMPONENTS target)

foreach(target IN ITEMS
    ScopeTest::symbolic ScopeTest::target ScopeTest::tool ScopeTest::extra)
  if(TARGET ${target})
    message(FATAL_ERROR "${target} was created despite the target conflict !")
  endif()
endforeach()
get_target_property(libraries ScopeTest INTERFACE_LINK_LIBRARIES)
if(NOT libraries STREQUAL "EXISTING")
  message(FATAL_ERROR "ScopeTest has wrong link libraries '${libraries}' !")
endif()
