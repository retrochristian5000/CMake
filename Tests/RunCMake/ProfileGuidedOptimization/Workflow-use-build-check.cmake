# GCC: -Wmissing-profile, Clang: -Wprofile-instr-missing,
# MSVC: C4961
if(actual_stdout MATCHES "profile count data file not found|profile-instr-missing|no profile data|No profile data was merged")
  set(RunCMake_TEST_FAILED "Profile data was not found:\n${actual_stdout}")
endif()
