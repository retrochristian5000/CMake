execute_process(COMMAND ${DUMP_COMMAND} ${DUMP_ARGS} ${TEST_LIBRARY_PATH}
  RESULT_VARIABLE RESULT
  OUTPUT_VARIABLE OUTPUT
  ERROR_VARIABLE ERROR
)

if(NOT "${RESULT}" STREQUAL "0")
  message(FATAL_ERROR "${DUMP_COMMAND} failed [${RESULT}] [${OUTPUT}] [${ERROR}]")
endif()


if(NOT "${OUTPUT}" MATCHES "(cudaRegisterLinkedBinary|.*UNDEF.*cudaRegisterLinkedBinary)")
  message(FATAL_ERROR
    "cudaRegisterLinkedBinary not found to be undefined, device linking occurred")
endif()
