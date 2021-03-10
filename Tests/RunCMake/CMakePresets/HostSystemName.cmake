include(${CMAKE_CURRENT_LIST_DIR}/TestVariable.cmake)

set(host_system_name "${CMAKE_HOST_SYSTEM_NAME}")
if(host_system_name STREQUAL "Darwin")
  set(host_system_name "macOS")
endif()
test_variable(TEST_HOST_SYSTEM_NAME "" "${host_system_name}")
