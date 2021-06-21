cmake_minimum_required(VERSION 3.18)
project(ConsumeCompileFeatures2 CXX CUDA)

# Compile C++ object as C++11 or higher in a way that does
# not influence the CUDA standard selection below.
add_library(CxxObjects OBJECT static.cpp)
if(CMAKE_CXX_STANDARD_DEFAULT STREQUAL "98")
  set_property(TARGET CxxObjects PROPERTY CXX_STANDARD 11)
endif()

# Goal for this example:
# Show that CXX default isn't used for CUDA when the
# value is higher than the max supported CUDA standard
set(CMAKE_CXX_STANDARD_DEFAULT "23")

add_library(CudaConsumeLib STATIC static.cu)
target_compile_features(CudaConsumeLib PUBLIC cxx_nullptr)
target_link_libraries(CudaConsumeLib PRIVATE CxxObjects)

add_executable(CudaConsumeCompileFeatures2 main.cu)
target_link_libraries(CudaConsumeCompileFeatures2 PRIVATE CudaConsumeLib)
