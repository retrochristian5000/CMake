
enable_language (C)
include(CheckSourceRuns)

check_source_runs(C "int main() {return 0;}" SHOULD_BUILD)
if(NOT SHOULD_BUILD)
  message(SEND_ERROR "Test fail for valid C source.")
endif()
