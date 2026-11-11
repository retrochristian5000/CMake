function(expect_global TARGET VALUE)
  if(NOT TARGET "${TARGET}")
    message(SEND_ERROR "${TARGET} missing !")
    return()
  endif()
  get_target_property(global "${TARGET}" IMPORTED_GLOBAL)
  if((VALUE AND NOT global) OR (NOT VALUE AND global))
    message(SEND_ERROR "${TARGET} has wrong IMPORTED_GLOBAL '${global}' !")
  endif()
endfunction()

function(expect_property TARGET PROPERTY DESCRIPTION)
  get_target_property(actual ${TARGET} ${PROPERTY})
  if(NOT "${actual}" STREQUAL "${ARGN}")
    message(SEND_ERROR "${TARGET} has wrong ${DESCRIPTION} '${actual}' !")
  endif()
endfunction()

function(expect_scope_targets VALUE)
  expect_global(ScopeTest::target "${VALUE}")
  expect_global(ScopeTest::symbolic "${VALUE}")
  expect_global(ScopeTest::tool "${VALUE}")
  expect_global(ScopeTest "${VALUE}")
  expect_property(ScopeTest::symbolic SYMBOLIC "symbolic flag" "TRUE")
  expect_property(
    ScopeTest::target INTERFACE_COMPILE_DEFINITIONS "definitions"
    "$<$<CONFIG:debug>:SCOPE_TEST=1>")
  expect_property(
    ScopeTest INTERFACE_LINK_LIBRARIES
    "default components" "ScopeTest::target")
endfunction()
