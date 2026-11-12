cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)
include(ImportScopeHelpers.cmake)

find_package(ScopeTest 1.0 CONFIG GLOBAL REQUIRED)
find_package(ScopeTest 2.0 CONFIG QUIET)
if(ScopeTest_FOUND)
  message(SEND_ERROR "ScopeTest 2.0 should not have been found !")
endif()
expect_scope_targets(TRUE)
