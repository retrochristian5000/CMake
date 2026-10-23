enable_language(C)
add_library(foo OBJECT foo.h)
set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION "Generate")
