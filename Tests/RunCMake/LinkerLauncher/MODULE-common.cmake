enable_language(C)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
add_library(foo MODULE foo.c)
target_compile_definitions(foo PRIVATE EXPORT=1)
