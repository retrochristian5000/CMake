include(Workflow.cmake)
# Only the library is instrumented, but linking the executable
# needs the profiling runtime library.
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION GENERATE)
