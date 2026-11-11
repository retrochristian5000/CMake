include(ImportScopeHelpers.cmake)

add_subdirectory(ImportScope ImportScope1)
add_subdirectory(ImportScope ImportScope2)

if(expect_global)
  expect_scope_targets(TRUE)
  set(import_args "")
  set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL OFF)
  add_subdirectory(ImportScope ImportScope3)
  find_package(ScopeTest CONFIG REQUIRED COMPONENTS extra)
  expect_scope_targets(TRUE)
else()
  foreach(target IN ITEMS
      ScopeTest::target ScopeTest::symbolic ScopeTest::tool ScopeTest
      ScopeTest::extra)
    if(TARGET ${target})
      message(SEND_ERROR "${target} should not be visible in the parent !")
    endif()
  endforeach()
endif()
