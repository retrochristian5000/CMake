cmake_minimum_required(VERSION 3.18)

project(Library C)

add_library(Library STATIC library.c library.h)

target_include_directories(Library INTERFACE .)

target_compile_options(Library PUBLIC -fshort-enums)
target_compile_definitions(Library INTERFACE USE_LIBRARY)
