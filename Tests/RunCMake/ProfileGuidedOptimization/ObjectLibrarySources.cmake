enable_language(C)
# Only the object library is instrumented, but linking the executable using
# its objects needs the profiling runtime library.
add_library(foo OBJECT foo.c)
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION GENERATE)
add_executable(main main.c $<TARGET_OBJECTS:foo>)

include(PGORuntime.cmake)
