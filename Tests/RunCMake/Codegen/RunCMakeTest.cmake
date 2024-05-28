include(RunCMake)

function(run_codegen case)
  set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/${case}-build)

  run_cmake(${case})

  set(RunCMake_TEST_NO_CLEAN 1)

  run_cmake_command(${case}-build ${CMAKE_COMMAND} --build . --target codegen --config Debug)
  run_cmake_command(${case}-test ${CMAKE_CTEST_COMMAND} -C Debug --no-tests=ignore)
endfunction()

# Builds codegen target when there are no custom commands marked codegen
run_codegen("no-codegen")

# Ensures codegen builds minimal build graphs
run_codegen("min-graph-1")
run_codegen("min-graph-2")
run_codegen("min-graph-3")

# Handle specific cases that can affect codegen
run_codegen("add-dependencies")
run_codegen("add-custom-command-depends")
run_codegen("byproducts")

# Error handling
run_cmake("implicit-depends")
run_cmake("implicit-depends-append")
run_cmake("no-output")
