include(RunCMake)

if(NOT CMAKE_GENERATOR_NO_COMPILER_ENV)
  run_cmake(CMP0131-WARN)
  run_cmake(CMP0131-OLD)
  run_cmake(CMP0131-NEW)
endif()
