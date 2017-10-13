include(RunCMake)

run_cmake(MissingDetails)
run_cmake(DirectIgnoresDetails)
run_cmake(FirstDetailsWin)
run_cmake(DownloadTwice)
run_cmake(SameGenerator)
run_cmake(VarDefinitions)
run_cmake(GetProperties)
run_cmake(DirOverrides)

# We need to pass through CMAKE_GENERATOR and CMAKE_MAKE_PROGRAM
# to ensure the test can run on machines where the build tool
# isn't on the PATH. Some build slaves explicitly test with such
# an arrangement (notably Ninja with spaces in the path).
run_cmake_command(ScriptMode
    ${CMAKE_COMMAND}
    -DCMAKE_GENERATOR=${RunCMake_GENERATOR}
    -DCMAKE_MAKE_PROGRAM=${RunCMake_MAKE_PROGRAM}
    -P ${CMAKE_CURRENT_LIST_DIR}/ScriptMode.cmake
)
