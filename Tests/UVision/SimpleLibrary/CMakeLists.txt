cmake_minimum_required(VERSION 3.20)
include(../default.cmake)

set(TARGET SimpleLibrary)

project(${TARGET} C)

add_library(${TARGET} STATIC library.c library.h)
