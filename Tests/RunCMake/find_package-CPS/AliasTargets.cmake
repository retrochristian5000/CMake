cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)
include(ImportScopeHelpers.cmake)

add_library(target INTERFACE)
add_library(ScopeTest::target ALIAS target)
add_library(ScopeTest::symbolic ALIAS target)
add_executable(tool IMPORTED)
add_executable(ScopeTest::tool ALIAS tool)
add_library(ScopeTest INTERFACE)
set_property(TARGET target PROPERTY INTERFACE_COMPILE_DEFINITIONS EXISTING)

find_package(ScopeTest CONFIG GLOBAL REQUIRED COMPONENTS target)
expect_property(target INTERFACE_COMPILE_DEFINITIONS "definitions" "EXISTING")
if(TARGET ScopeTest::extra)
  message(SEND_ERROR "ScopeTest::extra should not have been imported !")
endif()

add_library(ScopeDep::target ALIAS target)
find_package(ScopeReq CONFIG REQUIRED)
