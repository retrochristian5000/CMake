if(EXISTS "")
  message(FATAL_ERROR "EXISTS \"\" should not exist")
endif()

if (UNIX AND NOT MSYS)
  execute_process(COMMAND id -u $ENV{USER} OUTPUT_VARIABLE uid OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(uid STREQUAL "0")
    # if real user is root, tests are irrelevant
    return()
  endif()

  file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/file.txt")
  file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/dir")

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/file.txt" "foo")
  file(CHMOD "${CMAKE_CURRENT_BINARY_DIR}/file.txt" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dir")
  file(CHMOD "${CMAKE_CURRENT_BINARY_DIR}/dir" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

  block(SCOPE_FOR POLICIES)
    cmake_policy(SET CMP0156 OLD)
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/file.txt")
      message(FATAL_ERROR "EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/file.txt\" should not return true with CMP0156 set to OLD")
    endif()
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/dir")
      message(FATAL_ERROR "EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/dir\" should not return true with CMP0156 set to OLD")
    endif()

    cmake_policy(SET CMP0156 NEW)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/file.txt")
      message(FATAL_ERROR "EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/file.txt\" should return true with CMP0156 set to NEW")
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/dir")
      message(FATAL_ERROR "EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/dir\" should return true with CMP0156 set to NEW")
    endif()
  endblock()
endif()
