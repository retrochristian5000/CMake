cmake_minimum_required(VERSION 3.18)
project(ConsumeCompileFeatures2 CXX CUDA)
#Goal for this example:

#Show that CXX default isn't used for CUDA when the
#value is higher than the max supported CUDA standard
set(CMAKE_CXX_STANDARD_DEFAULT "23")

add_library(CudaConsumeLib STATIC static.cpp static.cu)
target_compile_features(CudaConsumeLib PUBLIC cxx_nullptr)

add_executable(CudaConsumeCompileFeatures2 main.cu)
target_link_libraries(CudaConsumeCompileFeatures2 PRIVATE CudaConsumeLib)
