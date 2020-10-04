configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sourcefile.txt
    ${CMAKE_CURRENT_BINARY_DIR}/sourcefile-use-source-permissions.txt
    USE_SOURCE_PERMISSIONS
)

if (UNIX)
  execute_process(COMMAND bash -c
      "stat -c '%a' ${CMAKE_CURRENT_SOURCE_DIR}/sourcefile.txt"
    OUTPUT_VARIABLE output1
  )
  execute_process(COMMAND bash -c
      "stat -c '%a' ${CMAKE_CURRENT_BINARY_DIR}/sourcefile-use-source-permissions.txt"
    OUTPUT_VARIABLE output2
  )
  if (NOT output1 EQUAL output2)
    message(FATAL_ERROR "configure file has different permissions source "
        "permissions: ${output1} generated permissions: ${output2}")
  endif()
endif()
