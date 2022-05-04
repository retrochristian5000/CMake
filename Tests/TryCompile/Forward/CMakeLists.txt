cmake_minimum_required(VERSION 3.24)
project(TryCompileForward C)

if(NOT DEFINED SHOULD_BE_DEFINED)
  message(FATAL_ERROR "SHOULD_BE_DEFINED was not forwarded by try_compile!")
endif()
