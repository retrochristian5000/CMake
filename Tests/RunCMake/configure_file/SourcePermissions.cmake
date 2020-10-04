configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sourcefile.txt
    ${CMAKE_CURRENT_BINARY_DIR}/sourcefile-source-permissions.txt
    FILE_PERMISSIONS
        OWNER_READ OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ
)

if (UNIX)
  execute_process(COMMAND bash -c
      "stat -c '%a' ${CMAKE_CURRENT_BINARY_DIR}/sourcefile-source-permissions.txt"
    OUTPUT_VARIABLE output
  )
  if (NOT output EQUAL "554")
    message(FATAL_ERROR "configure file has different permissions than "
        "desired, generated permissions: ${output}")
  endif()
endif()
