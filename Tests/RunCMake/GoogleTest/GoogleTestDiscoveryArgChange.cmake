enable_language(CXX)
include(GoogleTest)

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(xcode_sign_adhoc.cmake)

add_executable(fake_gtest fake_gtest.cpp)
xcode_sign_adhoc(fake_gtest)

gtest_discover_tests(
  fake_gtest
  TEST_FILTER "${TEST_FILTER}*"
)
