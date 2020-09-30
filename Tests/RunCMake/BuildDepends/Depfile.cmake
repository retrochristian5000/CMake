enable_language(C)

add_subdirectory(Depfile)

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/check-$<LOWER_CASE:$<CONFIG>>.cmake CONTENT "
function(check_exists file)
  if(NOT EXISTS \"\${file}\")
    string(APPEND RunCMake_TEST_FAILED \"\${file} does not exist\\n\")
  endif()
  set(RunCMake_TEST_FAILED \"\${RunCMake_TEST_FAILED}\" PARENT_SCOPE)
endfunction()

function(check_not_exists file)
  if(EXISTS \"\${file}\")
    string(APPEND RunCMake_TEST_FAILED \"\${file} exists\\n\")
  endif()
  set(RunCMake_TEST_FAILED \"\${RunCMake_TEST_FAILED}\" PARENT_SCOPE)
endfunction()

set(check_pairs
  \"${CMAKE_BINARY_DIR}/Depfile/cc.c|${CMAKE_BINARY_DIR}/Depfile/ccdep.txt\"
  \"$<TARGET_FILE:exe>|${CMAKE_BINARY_DIR}/Depfile/exedep.txt\"
  \"$<TARGET_FILE:lib>|${CMAKE_BINARY_DIR}/Depfile/libdep.txt\"
  )

if(check_step EQUAL 3)
  check_not_exists(\"${CMAKE_BINARY_DIR}/Depfile/ccstamp.txt\")
  check_not_exists(\"${CMAKE_BINARY_DIR}/Depfile/exestamp.txt\")
  check_not_exists(\"${CMAKE_BINARY_DIR}/Depfile/libstamp.txt\")
else()
  check_exists(\"${CMAKE_BINARY_DIR}/Depfile/ccstamp.txt\")
  check_exists(\"${CMAKE_BINARY_DIR}/Depfile/exestamp.txt\")
  check_exists(\"${CMAKE_BINARY_DIR}/Depfile/libstamp.txt\")
endif()
")
