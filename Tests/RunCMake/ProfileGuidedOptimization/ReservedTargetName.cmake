enable_language(C)
set(CMAKE_PROFILE_GUIDED_OPTIMIZATION GENERATE)
add_library(foo STATIC foo.c)
add_custom_target(cmake_pgo_merge)
