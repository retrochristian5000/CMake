enable_language(C)

add_library(lib1 STATIC empty.c)
target_headers(lib1 PRIVATE HEADER_SET a HEADERS h1.h)
install(TARGETS lib1 EXPORT a)

add_library(lib2 STATIC empty.c)
target_headers(lib2 INTERFACE HEADER_SET a HEADERS h1.h)
install(TARGETS lib2)
