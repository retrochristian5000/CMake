cmake_minimum_required(VERSION 3.24)
project(TryCompileNoForward C)

if(DEFINED SHOULD_NOT_BE_DEFINED)
  message(FATAL_ERROR "SHOULD_NOT_BE_DEFINED was forwarded by try_compile!")
endif()
