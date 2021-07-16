enable_language(C)

add_library(lib1 STATIC empty.c)
target_headers(lib1 PRIVATE HEADER_SET a HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/h1.h")
