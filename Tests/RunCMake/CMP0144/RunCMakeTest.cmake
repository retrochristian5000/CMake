include(RunCMake)

function(run_cmp0144 name)
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${name}-build)
  run_cmake(${name})
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(${name}-build ${CMAKE_COMMAND} --build . --config Debug)
endfunction()

run_cmp0144(CMP0144-WARN)
run_cmp0144(CMP0144-WARN-var)
run_cmp0144(CMP0144-OLD)
run_cmp0144(CMP0144-NEW)
