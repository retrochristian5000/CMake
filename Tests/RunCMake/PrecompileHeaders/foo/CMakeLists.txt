add_library(foo ../foo.c)
target_include_directories(foo PUBLIC ../include)
target_precompile_headers(foo REUSE_FROM empty)

set_target_properties(foo PROPERTIES PREFIX "lib" IMPORT_PREFIX "lib")
