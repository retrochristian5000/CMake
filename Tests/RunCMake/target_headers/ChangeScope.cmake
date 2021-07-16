enable_language(C)

add_library(lib1 STATIC empty.c)
target_headers(lib1 INTERFACE HEADER_SET a)
target_headers(lib1 PUBLIC HEADER_SET a)
