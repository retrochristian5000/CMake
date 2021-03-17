cmake_minimum_required(VERSION 3.20)
include(../default.cmake)

set(TARGET BuildEvents)
project(${TARGET} C)

add_library(${TARGET} STATIC foo.c foo.h)

add_custom_command(
  TARGET ${TARGET} PRE_BUILD
  COMMAND "echo BuildEvent: PRE_BUILD1> TestLog.txt"
  COMMAND "echo BuildEvent: PRE_BUILD2>> TestLog.txt")

add_custom_command(
  TARGET ${TARGET} POST_BUILD
  COMMAND "echo BuildEvent: POST_BUILD1>> TestLog.txt")

add_custom_command(
  TARGET ${TARGET} POST_BUILD
  COMMAND "echo BuildEvent: POST_BUILD2>> TestLog.txt")
