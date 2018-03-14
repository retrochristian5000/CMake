#include <chrono>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv)
{
  // Note: GoogleTest.cmake doesn't actually depend on Google Test as such;
  // it only requires that we produce output in the expected format when
  // invoked with --gtest_list_tests. Thus, we fake that here. This allows us
  // to test the module without actually needing Google Test.
  if (argc > 1 && std::string(argv[1]) == "--gtest_list_tests") {
    std::cout << "timeout." << std::endl;
    std::cout << "  case" << std::endl;
#ifdef discoverySleepSec
    std::this_thread::sleep_for(std::chrono::seconds(discoverySleepSec));
#endif
    return 0;
  }

#ifdef sleepSec
  std::this_thread::sleep_for(std::chrono::seconds(sleepSec));
#endif

  return 0;
}
