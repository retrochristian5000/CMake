cmake_minimum_required(VERSION 3.18)
project(CudaOnlyAll CUDA)

if(CMAKE_CUDA_COMPILER_ID STREQUAL "NVIDIA" AND
  CMAKE_CUDA_COMPILER_VERSION VERSION_GREATER_EQUAL 11.5.0)

  add_executable(CudaOnlyAll main.cu)
  set_property(TARGET CudaOnlyAll PROPERTY CUDA_ARCHITECTURES all)
  target_compile_options(CudaOnlyAll PRIVATE -Wno-deprecated-gpu-targets)

  set(CMAKE_CUDA_ARCHITECTURES all-major)
  add_executable(CudaOnlyAllMajor main.cu)
  target_compile_options(CudaOnlyAllMajor PRIVATE -Wno-deprecated-gpu-targets)

else()
  add_executable(CudaOnlyAll main.cu)
endif()

