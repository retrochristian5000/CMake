cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)
include(ImportScopeHelpers.cmake)

set(expect_global FALSE)
add_subdirectory(ImportScope ImportScope1)

set(import_args GLOBAL)
set(expect_global TRUE)
add_subdirectory(ImportScope ImportScope2)
expect_scope_targets(TRUE)
expect_global(ScopeTest::extra TRUE)
