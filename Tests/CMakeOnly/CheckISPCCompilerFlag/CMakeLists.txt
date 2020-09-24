cmake_minimum_required(VERSION 3.16)
project(CheckISPCCompilerFlag ISPC)
include(CheckISPCCompilerFlag)

check_ispc_compiler_flag(-fake_flag HAS_BAD_COMPILER_FLAG)
if(HAS_BAD_COMPILER_FLAG)
message(SEND_ERROR "Test fail: -fake_flag didn't cause check_ispc_compiler_flag failure")
endif()

check_ispc_compiler_flag(-DFOO HAS_COMPILER_FLAG)
if(NOT HAS_COMPILER_FLAG)
  message(SEND_ERROR "Test fail: -DFoo caused a check_ispc_compiler_flag failure")
endif()
