execute_process(
  COMMAND ${CMAKE_COMMAND} -E bin2c ${ARGS} "${INPUT_FILE}"
  OUTPUT_FILE "${OUTPUT_FILE}"
  RESULT_VARIABLE result
  )
if(result)
  message(FATAL_ERROR "bin2c failed to run")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E compare_files --ignore-eol "${EXPECTED_OUTPUT_FILE}" "${OUTPUT_FILE}"
  RESULT_VARIABLE result
  )
if(result)
  file(READ "${EXPECTED_OUTPUT_FILE}" expected_output)
  file(READ "${OUTPUT_FILE}" actual_output)
  string(REPLACE "\n" "\n  " formatted_expected_output "${expected_output}")
  string(REPLACE "\n" "\n  " formatted_actual_output "${actual_output}")
  set(formatted_binary_input)
  file(READ "${INPUT_FILE}" binary_contents HEX)
  string(LENGTH "${binary_contents}" binary_length)
  if(binary_length LESS 256)
    set(formatted_binary_input "\nHexadecimal contents of ${INPUT_FILE}:\n  ${binary_contents}")
  endif()
  message(FATAL_ERROR "${OUTPUT_FILE} does not match ${EXPECTED_OUTPUT_FILE}.\nExpected output:\n  ${formatted_expected_output}\nActual output:\n  ${formatted_actual_output}${formatted_binary_input}")
endif()
