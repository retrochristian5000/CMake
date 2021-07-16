enable_language(C)

add_library(lib1 STATIC empty.c)
target_headers(lib1 INTERFACE DIRECTORY dir1)
target_headers(lib1 INTERFACE DIRECTORY dir2)
