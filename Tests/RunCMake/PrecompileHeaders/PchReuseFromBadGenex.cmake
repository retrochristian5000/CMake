cmake_minimum_required(VERSION 3.15)
project(PchReuseFromBadGenex C)

if(CMAKE_C_COMPILE_OPTIONS_USE_PCH)
  add_definitions(-DHAVE_PCH_SUPPORT)
endif()

# Add this before the target from which we will reuse the PCH
# to test that generators can handle reversed ordering.
add_library(foo foo.c)
target_include_directories(foo PUBLIC include)

add_library(empty empty.c)
target_precompile_headers(empty PRIVATE
  <stdio.h>
  <string.h>
)
target_include_directories(empty PUBLIC include)

target_precompile_headers(foo REUSE_FROM $<NOTGENEX>)
