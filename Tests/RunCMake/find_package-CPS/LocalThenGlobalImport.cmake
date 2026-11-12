cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)
include(ImportScopeHelpers.cmake)

find_package(ScopeTest CONFIG REQUIRED)
expect_scope_targets(FALSE)

find_package(ScopeTest CONFIG GLOBAL REQUIRED)
expect_scope_targets(FALSE)
expect_global(ScopeTest::extra FALSE)

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL TRUE)
find_package(ScopeTest CONFIG REQUIRED)
expect_scope_targets(FALSE)

set(import_args GLOBAL)
set(expect_global FALSE)
add_subdirectory(ImportScope)
