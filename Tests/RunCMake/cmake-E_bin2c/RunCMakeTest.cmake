include(RunCMake)

function(run_bin2c name)
  string(REPLACE ";" "\\;" ARGS "${ARGN}")
  run_cmake_command(${name}
    "${CMAKE_COMMAND}"
    "-DINPUT_FILE=${RunCMake_TEST_BINARY_DIR}/${name}.bin"
    "-DOUTPUT_FILE=${RunCMake_TEST_BINARY_DIR}/${name}.c"
    "-DEXPECTED_OUTPUT_FILE=${RunCMake_SOURCE_DIR}/${name}.c.txt"
    "-DARGS=${ARGS}"
    -P "${RunCMake_SOURCE_DIR}/run_bin2c.cmake"
    )
endfunction()

set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/generate_binaries-build")
run_cmake(generate_binaries)
set(RunCMake_TEST_NO_CLEAN 1)
run_cmake_command(generate_binaries-build "${CMAKE_COMMAND}" --build .)
run_bin2c(basic)
run_bin2c(st -st)
run_bin2c(static --static)
run_bin2c(c -c)
run_bin2c(const --const)
run_bin2c(static_const --static --const)
run_bin2c(n -n embedded_file)
run_bin2c(name --name embedded_file --const)
run_bin2c(very_long)
run_bin2c(empty)
run_bin2c(text_lf)
run_bin2c(text_crlf)
run_bin2c(text_align)

run_cmake_command(invalid_arg "${CMAKE_COMMAND}" -E bin2c --invalid-arg "${RunCMake_TEST_BINARY_DIR}/basic.bin")
run_cmake_command(arg_order "${CMAKE_COMMAND}" -E bin2c "${RunCMake_TEST_BINARY_DIR}/basic.bin" --static)
run_cmake_command(no_filename "${CMAKE_COMMAND}" -E bin2c)
run_cmake_command(no_filename_with_arg "${CMAKE_COMMAND}" -E bin2c --static)
run_cmake_command(noexist "${CMAKE_COMMAND}" -E bin2c "${RunCMake_TEST_BINARY_DIR}/noexist.bin")
