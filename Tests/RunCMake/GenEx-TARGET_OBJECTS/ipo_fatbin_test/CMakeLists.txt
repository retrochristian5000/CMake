cmake_minimum_required(VERSION 4.3)
project(ipo_fatbin_test CUDA)

add_library(ipo_fatbin_test OBJECT kernel.cu)
set_target_properties(ipo_fatbin_test PROPERTIES
  CUDA_SEPARABLE_COMPILATION ON
  CUDA_FATBIN_COMPILATION ON
  POSITION_INDEPENDENT_CODE ON
  INTERPROCEDURAL_OPTIMIZATION ON
  )
