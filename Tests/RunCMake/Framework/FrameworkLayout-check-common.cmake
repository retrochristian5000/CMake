macro(check_plist key expect)
  execute_process(
    COMMAND plutil -extract "${key}" raw "${plist-file}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE actual
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(NOT "${actual}" STREQUAL "${expect}")
    string(CONCAT RunCMake_TEST_FAILED
      "Framework Info.plist key \"${key}\" has value:\n"
      "  \"${actual}\"\n"
      "but we expected:\n"
      "  \"${expect}\""
      )
  endif()
endmacro()

check_plist(CFBundleIdentifier MyFrameworkId)
check_plist(CFBundleName MyFrameworkBundleName)
check_plist(CFBundleVersion 3.2.1)
check_plist(CFBundleShortVersionString 3)
