cmake_minimum_required(VERSION 3.10)
project(TimeoutAfterMatchRepeat NONE)
enable_testing()

add_test(
  NAME RepeatTimeout
  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/Repeat.cmake"
)
set_property(TEST RepeatTimeout PROPERTY TIMEOUT 1)
set_property(TEST RepeatTimeout PROPERTY TIMEOUT_AFTER_MATCH 5 "Test started")
