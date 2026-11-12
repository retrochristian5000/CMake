cmake_minimum_required(VERSION 4.0)

include(Setup.cmake)
include(ImportScopeHelpers.cmake)

find_package(ScopeOuter CONFIG GLOBAL REQUIRED)
expect_scope_targets(TRUE)
expect_global(ScopeReq::target TRUE)
expect_global(ScopeDep::target TRUE)

find_package(ConfigFirst CONFIG REQUIRED)
expect_global(ConfigFirst::target FALSE)
