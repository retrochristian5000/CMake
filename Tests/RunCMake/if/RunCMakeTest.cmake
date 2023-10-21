include(RunCMake)

unset(uid)
unset(status)
if(UNIX AND NOT MSYS)
  # permissions and symbolic links are broken on MSYS
  set(ID "id")
  if (CMAKE_SYSTEM_NAME STREQUAL "SunOS" AND EXISTS "/usr/xpg4/bin/id")
    set (ID "/usr/xpg4/bin/id")
  endif()
  # if real user is root, tests are irrelevant
  execute_process(COMMAND ${ID} -u $ENV{USER} OUTPUT_VARIABLE uid ERROR_QUIET
    RESULT_VARIABLE status OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()


run_cmake(InvalidArgument1)
block(SCOPE_FOR VARIABLES)
  unset(CHECK_CMP0156)
  if(UNIX AND NOT MSYS AND NOT status AND NOT uid STREQUAL "0")
    set(CHECK_CMP0156 TRUE)
    file(READ "${RunCMake_SOURCE_DIR}/exists-stderr-input.txt" RunCMake_TEST_EXPECT_stderr)
  endif()
  run_cmake_with_options(exists -DCHECK_CMP0156=${CHECK_CMP0156})
endblock()
if(NOT MSYS AND NOT status AND NOT uid STREQUAL "0")
  run_cmake(FilePermissions)
endif()
run_cmake(IsDirectory)
run_cmake(IsDirectoryLong)
run_cmake(duplicate-deep-else)
run_cmake(duplicate-else)
run_cmake(duplicate-else-after-elseif)
run_cmake(elseif-message)
run_cmake(misplaced-elseif)

run_cmake(unbalanced-parenthesis)

run_cmake(MatchesSelf)
run_cmake(IncompleteMatches)
run_cmake(IncompleteMatchesFail)

run_cmake(TestNameThatExists)
run_cmake(TestNameThatDoesNotExist)

run_cmake_script(AndOr)
