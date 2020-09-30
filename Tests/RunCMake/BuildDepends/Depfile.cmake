enable_language(C)

add_custom_command(
  OUTPUT cc.c
  DEPFILE cc.c.d
  COMMAND ${CMAKE_COMMAND} -DOUTFILE=cc.c -DINFILE=ccdep.txt -DDEPFILE=cc.c.d -DSTAMPFILE=ccstamp.txt -P ${CMAKE_CURRENT_LIST_DIR}/WriteDepfile.cmake
  )
add_custom_target(cc ALL DEPENDS cc.c)

add_custom_command(
  OUTPUT exe.c
  DEPFILE exe.c.d
  COMMAND ${CMAKE_COMMAND} -DOUTFILE=exe.c -DINFILE=exedep.txt -DDEPFILE=exe.c.d -DSTAMPFILE=exestamp.txt -P ${CMAKE_CURRENT_LIST_DIR}/WriteDepfile.cmake
  )
add_executable(exe ${CMAKE_BINARY_DIR}/exe.c)

add_custom_command(
  OUTPUT lib.c
  DEPFILE lib.c.d
  COMMAND ${CMAKE_COMMAND} -DOUTFILE=lib.c -DINFILE=libdep.txt -DDEPFILE=lib.c.d -DSTAMPFILE=libstamp.txt -P ${CMAKE_CURRENT_LIST_DIR}/WriteDepfile.cmake
  )
add_library(lib STATIC ${CMAKE_BINARY_DIR}/lib.c)

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
  \"${CMAKE_BINARY_DIR}/cc.c|${CMAKE_BINARY_DIR}/ccdep.txt\"
  \"$<TARGET_FILE:exe>|${CMAKE_BINARY_DIR}/exedep.txt\"
  \"$<TARGET_FILE:lib>|${CMAKE_BINARY_DIR}/libdep.txt\"
  )

if(check_step EQUAL 3)
  check_not_exists(\"${CMAKE_BINARY_DIR}/ccstamp.txt\")
  check_not_exists(\"${CMAKE_BINARY_DIR}/exestamp.txt\")
  check_not_exists(\"${CMAKE_BINARY_DIR}/libstamp.txt\")
else()
  check_exists(\"${CMAKE_BINARY_DIR}/ccstamp.txt\")
  check_exists(\"${CMAKE_BINARY_DIR}/exestamp.txt\")
  check_exists(\"${CMAKE_BINARY_DIR}/libstamp.txt\")
endif()
")
