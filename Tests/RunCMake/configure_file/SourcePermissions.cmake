configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sourcefile.txt
    ${CMAKE_CURRENT_BINARY_DIR}/sourcefile-source-permissions.txt
    FILE_PERMISSIONS
        OWNER_READ OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ
)

if (UNIX)
  find_program(STAT_EXECUTABLE NAMES stat)
  if(NOT STAT_EXECUTABLE)
    return()
  endif()

  if (APPLE)
    execute_process(COMMAND "${STAT_EXECUTABLE}" -f %A "${CMAKE_CURRENT_BINARY_DIR}/sourcefile-source-permissions.txt"
      OUTPUT_VARIABLE output
    )
  else()
    execute_process(COMMAND "${STAT_EXECUTABLE}" -c %a "${CMAKE_CURRENT_BINARY_DIR}/sourcefile-source-permissions.txt"
      OUTPUT_VARIABLE output
    )
  endif()

  if (NOT output EQUAL "554")
    message(FATAL_ERROR "configure file has different permissions than "
        "desired, generated permissions: ${output}")
  endif()

endif()
