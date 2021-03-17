cmake_minimum_required(VERSION 3.20)
include(../default.cmake)

set(TARGET CustomCommand)

project(${TARGET} C)

add_custom_command(
  OUTPUT foo.c
  COMMAND echo void foo() { }>foo.c
)

add_library(${TARGET} STATIC foo.c)
