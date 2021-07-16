enable_language(C)

add_library(lib1 STATIC empty.c)
target_headers(lib1 PRIVATE HEADER_SET a HEADERS h1.h)
set_property(TARGET lib1 PROPERTY HEADER_SETS)
target_headers(lib1 PRIVATE HEADER_SET a HEADERS h2.h)
