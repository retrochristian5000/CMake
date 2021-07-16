enable_language(C)

add_library(lib1 STATIC empty.c)
set_property(TARGET lib1 PROPERTY HEADER_SETS "a")

# Error happens at configure-time, so this doesn't help.
target_headers(lib1 PRIVATE HEADER_SET a)
