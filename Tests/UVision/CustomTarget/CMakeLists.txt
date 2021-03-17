cmake_minimum_required(VERSION 3.20)
include(../default.cmake)

set(TARGET CustomTarget)

project(${TARGET} NONE)

add_custom_target(
  ${TARGET}
  COMMAND "echo Built CustomTarget> TestLog.txt"
)
