cmake_minimum_required(VERSION 3.17)
project(SharedRuntimeViaCUDAFlags NONE)

set(CMAKE_CUDA_FLAGS "")
string(APPEND CMAKE_CUDA_FLAGS "-cudart shared")

enable_language(CUDA)

add_executable(CudaOnlySharedRuntimeViaCUDAFlags main.cu)
