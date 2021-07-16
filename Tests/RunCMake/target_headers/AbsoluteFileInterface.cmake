enable_language(C)

add_library(lib1 STATIC empty.c)
target_headers(lib1 INTERFACE HEADER_SET a HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/h1.h")
install(TARGETS lib1 HEADER_SET a)
