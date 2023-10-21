
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt" "foo")
file(CHMOD "${CMAKE_CURRENT_BINARY_DIR}/readable.txt" PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/writable.txt" "foo")
file(CHMOD "${CMAKE_CURRENT_BINARY_DIR}/writable.txt" PERMISSIONS OWNER_WRITE GROUP_WRITE WORLD_WRITE)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/executable.txt" "foo")
file(CHMOD "${CMAKE_CURRENT_BINARY_DIR}/executable.txt" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)


if(WIN32)
  # files are always readable and executable
  if(NOT IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt"
      OR IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/readable.txt\" failed")
  endif()

  if(NOT IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/executable.txt"
      OR IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/executable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/executable.txt\" failed")
  endif()
else()
  if(NOT IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt"
      OR IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt"
      OR IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/readable.txt\" failed")
  endif()

  if(NOT IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/writable.txt"
      OR IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/writable.txt"
      OR IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/writable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/writable.txt\" failed")
  endif()

  if(NOT IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/executable.txt"
      OR IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/executable.txt"
      OR IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/executable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/executable.txt\" failed")
  endif()
endif()

if(UNIX)
  #
  # Check that file permissions are on the real file, not the symbolic link
  #
  file(CREATE_LINK "${CMAKE_CURRENT_BINARY_DIR}/readable.txt"
    "${CMAKE_CURRENT_BINARY_DIR}/link-to-readable.txt"
    SYMBOLIC)

  file(CREATE_LINK "${CMAKE_CURRENT_BINARY_DIR}/writable.txt"
    "${CMAKE_CURRENT_BINARY_DIR}/link-to-writable.txt"
    SYMBOLIC)

  file(CREATE_LINK "${CMAKE_CURRENT_BINARY_DIR}/executable.txt"
    "${CMAKE_CURRENT_BINARY_DIR}/link-to-executable.txt"
    SYMBOLIC)

  if(NOT IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-readable.txt"
     OR IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-readable.txt"
     OR IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-readable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/link-to-readable.txt\" failed")
  endif()

  if(NOT IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-writable.txt"
     OR IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-writable.txt"
     OR IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-writable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/link-to-writable.txt\" failed")
  endif()

  if(NOT IS_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-executable.txt"
     OR IS_READABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-executable.txt"
     OR IS_WRITABLE "${CMAKE_CURRENT_BINARY_DIR}/link-to-executable.txt")
    message(FATAL_ERROR "checks on \"${CMAKE_CURRENT_BINARY_DIR}/link-to-executable.txt\" failed")
  endif()

  file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/link-to-readable.txt"
              "${CMAKE_CURRENT_BINARY_DIR}/link-to-writable.txt"
              "${CMAKE_CURRENT_BINARY_DIR}/link-to-executable.txt")
endif()

file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/readable.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/writable.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/executable.txt")
