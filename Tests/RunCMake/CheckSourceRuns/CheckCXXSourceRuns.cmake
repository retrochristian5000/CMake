
enable_language (CXX)
include(CheckSourceRuns)

check_source_runs(CXX [=[
  #include <vector>
  int main() {
    return 0;
  }
]=]
 SHOULD_BUILD)

if(NOT SHOULD_BUILD)
  message(SEND_ERROR "Test fail for valid CXX source.")
endif()
