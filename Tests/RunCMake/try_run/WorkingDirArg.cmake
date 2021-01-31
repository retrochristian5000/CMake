if(UNIX AND NOT APPLE)
  try_run(RUN_RESULT COMPILE_RESULT
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp ${CMAKE_CURRENT_SOURCE_DIR}/another_dir.c
    RUN_OUTPUT_VARIABLE OUTPUT_VARIABLE
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp/workdir
    )
else()
  try_run(RUN_RESULT COMPILE_RESULT
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp ${CMAKE_CURRENT_SOURCE_DIR}/src.c
    RUN_OUTPUT_VARIABLE OUTPUT_VARIABLE
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp/workdir
    )
endif()

if(RUN_RESULT)
  message(SEND_ERROR "try run failed with result: ${RUN_RESULT}")
endif()

if(UNIX AND NOT APPLE)
  if(NOT OUTPUT_VARIABLE MATCHES ".*/Tests/RunCMake/try_run/WorkingDirArg-build/CMakeTmp/workdir")
    message(SEND_ERROR "Actual output: ${OUTPUT_VARIABLE} Expected output: .*/Tests/RunCMake/try_run/WorkingDirArg-build/CMakeTmp/workdir")
  endif()
endif()
