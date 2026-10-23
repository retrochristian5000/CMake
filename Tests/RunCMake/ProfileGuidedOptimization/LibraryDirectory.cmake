include(Workflow.cmake)
# Only the library is instrumented, so the executable linking it uses the
# profile data directory of the library.
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION GENERATE)
set_property(TARGET foo PROPERTY PGO_DATA_DIRECTORY pgo-foo)
