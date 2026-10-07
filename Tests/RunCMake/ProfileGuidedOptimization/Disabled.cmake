enable_language(C)
set(CMAKE_PROFILE_GUIDED_OPTIMIZATION OFF)
add_library(foo STATIC foo.c)
add_library(bar STATIC foo.c)
set_property(TARGET bar PROPERTY PROFILE_GUIDED_OPTIMIZATION "")
# A generator expression may enable PGO only for some configurations.
add_library(baz STATIC foo.c)
set_property(TARGET baz PROPERTY PROFILE_GUIDED_OPTIMIZATION
  "$<$<CONFIG:NoSuchConfig>:INVALID>")
# Missing compiler support is not diagnosed when PGO is disabled.
unset(CMAKE_C_COMPILE_OPTIONS_PGO_GENERATE)
unset(CMAKE_C_COMPILE_OPTIONS_PGO_USE)
# The merge target name is only reserved when PGO is enabled.
add_custom_target(cmake_pgo_merge)
