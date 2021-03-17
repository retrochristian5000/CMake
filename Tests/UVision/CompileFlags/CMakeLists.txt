cmake_minimum_required(VERSION 3.20)
include(../default.cmake)

set(TARGET CompileFlags)
project(${TARGET} C)

add_library(${TARGET} STATIC foo.c)
