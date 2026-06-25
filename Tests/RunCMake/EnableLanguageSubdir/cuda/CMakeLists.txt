enable_language(CUDA)

add_library(cuda_lib STATIC src.cu)
target_include_directories(cuda_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET cuda_lib PROPERTY CUDA_ARCHITECTURES native)
