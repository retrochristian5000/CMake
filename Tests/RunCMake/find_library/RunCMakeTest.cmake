include(RunCMake)

run_cmake(Created)
run_cmake(FromPrefixPath)
run_cmake(FromPATHEnv)
if(UNIX AND NOT CYGWIN)
  run_cmake(LibArchLink)
  run_cmake(LibSymLink)
endif()
run_cmake(PrefixInPATH)
run_cmake(Required)
run_cmake(NO_CACHE)

run_cmake_command(FromScriptMode ${CMAKE_COMMAND} -P ${RunCMake_SOURCE_DIR}/FromScriptMode.cmake)
