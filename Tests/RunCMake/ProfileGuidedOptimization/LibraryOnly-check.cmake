# Only the library compiles its sources for profile guided optimization.
if(RunCMake_GENERATOR MATCHES "Visual Studio")
  file(READ "${RunCMake_TEST_BINARY_DIR}/main.vcxproj" vcxproj)
  if(vcxproj MATCHES "<WholeProgramOptimization>true")
    set(RunCMake_TEST_FAILED "main.vcxproj enables whole program optimization.")
  endif()
endif()
