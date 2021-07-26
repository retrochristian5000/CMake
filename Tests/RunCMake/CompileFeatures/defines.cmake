enable_language(@lang@)
try_run(run_result compile_result "${CMAKE_CURRENT_BINARY_DIR}" "@RunCMake_SOURCE_DIR_REAL@/defines.@ext@" RUN_OUTPUT_VARIABLE run_output)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/output.txt"
"standard_default=@standard_default@\n"
"extensions_default=@extensions_default@\n"
"@lang@ run output:\n${run_output}"
)
