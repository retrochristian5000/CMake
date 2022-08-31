set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
set(CMAKE_CUDA_ARCHITECTURES OFF)
project(SeparableCompCXXOnly LANGUAGES CXX CUDA)
add_executable(SeparableCompCXXOnly main.cpp)
