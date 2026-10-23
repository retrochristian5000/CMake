enable_language(C)
add_library(foo STATIC foo.c)
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION
  "$<$<CONFIG:Debug>:USE>$<$<CONFIG:Release>:INVALID>")
