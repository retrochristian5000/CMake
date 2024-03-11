set(SUBMIT_URL "https://-no-site-")
set(CTEST_SUBMIT_RETRY_COUNT 0 CACHE STRING "")
include(CTest)
file(WRITE ${CMAKE_BINARY_DIR}/CTestConfig.cmake [[
set(ENV{no_proxy} "$ENV{no_proxy},-no-site-")
]])
