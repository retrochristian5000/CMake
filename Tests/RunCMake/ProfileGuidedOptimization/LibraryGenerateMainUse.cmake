include(Workflow.cmake)
# The library is instrumented, so linking the executable needs the profiling
# runtime library even though the executable itself uses profile data.
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION GENERATE)
set_property(TARGET foo PROPERTY PGO_DATA_DIRECTORY pgo-foo)
set_property(TARGET main PROPERTY PROFILE_GUIDED_OPTIMIZATION USE)

# This case has no profile data for the executable, so tell GCC not to warn.
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(main PRIVATE -Wno-missing-profile)
endif()
