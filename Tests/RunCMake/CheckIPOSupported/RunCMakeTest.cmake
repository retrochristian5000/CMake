include(RunCMake)

run_cmake(unparsed-arguments)
run_cmake(user-lang-unknown)
run_cmake(default-lang-none)
run_cmake(not-supported-by-cmake)
run_cmake(not-supported-by-compiler)
run_cmake(save-to-result)
run_cmake(cmp0069-is-old)

run_cmake(CMP0138-WARN)
run_cmake(CMP0138-OLD)
run_cmake(CMP0138-NEW)

if(RunCMake_GENERATOR MATCHES "^Visual Studio 9 ")
  run_cmake(not-supported-by-generator)
endif()
