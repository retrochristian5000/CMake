cmake_policy(SET CMP0127 OLD) # this test covers OLD behavior
enable_language(C)

add_library(ext_cuda IMPORTED STATIC)
set_property(TARGET ext_cuda PROPERTY IMPORTED_LOCATION "/does_not_exist")
set_property(TARGET ext_cuda PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGES "CUDA")

add_executable(main empty.c)
target_link_libraries(main ext_cuda)
