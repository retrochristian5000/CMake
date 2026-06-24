include(RunCMake)

function(run_configure_and_build name)
  run_cmake(${name})
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/${name}-build")
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(${name}-build ${CMAKE_COMMAND} --build . --config Debug)
endfunction()

run_cmake(OUTPUT_FILE_SET-BadPattern)
run_cmake(OUTPUT_FILE_SET-BadType)
run_cmake(OUTPUT_FILE_SET-TypeMismatch)

run_cmake(RuleProperties)

run_configure_and_build(Simple)
run_configure_and_build(Global)
run_configure_and_build(Local)
run_configure_and_build(CustomTarget)
run_configure_and_build(Patterns)
run_configure_and_build(Configurators)
run_configure_and_build(DerivedRule)
