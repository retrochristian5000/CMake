enable_language(C)
set(CMAKE_PROFILE_GUIDED_OPTIMIZATION GENERATE)
# Clang writes the profile data of a binary to the profile data directory of
# only one of its instrumented objects.
add_library(foo STATIC foo.c)
set_property(TARGET foo PROPERTY PGO_DATA_DIRECTORY pgo-foo)
add_executable(main main.c)
target_link_libraries(main PRIVATE foo)
