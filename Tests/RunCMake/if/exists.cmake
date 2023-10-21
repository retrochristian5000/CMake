if(EXISTS "")
  message(FATAL_ERROR "EXISTS \"\" should not exist")
endif()

if (UNIX AND NOT MSYS)
  execute_process(COMMAND id -rnu OUTPUT_VARIABLE real_user OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(real_user STREQUAL "root")
    # tests are irrelevant in this case
    return()
  endif()

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/file.txt" "foo")
  file(CHMOD "${CMAKE_CURRENT_BINARY_DIR}/file.txt" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

  block(SCOPE_FOR POLICIES)
    cmake_policy(SET CMP0156 OLD)
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/file.txt")
      message(FATAL_ERROR "EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/file.txt\" should not return true with CMP0156 set to OLD")
    endif()

    cmake_policy(SET CMP0156 NEW)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/file.txt")
      message(FATAL_ERROR "EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/file.txt\" should return true with CMP0156 set to NEW")
    endif()
  endblock()

  file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/file.txt")
endif()
