cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)
include(ImportScopeHelpers.cmake)

find_package(ScopeTest CONFIG GLOBAL REQUIRED COMPONENTS target)
if(TARGET ScopeTest::extra)
  message(SEND_ERROR "ScopeTest::extra should not have been imported !")
endif()

set(import_args GLOBAL)
set(expect_global TRUE)
add_subdirectory(ImportScope)
expect_global(ScopeTest::extra TRUE)
