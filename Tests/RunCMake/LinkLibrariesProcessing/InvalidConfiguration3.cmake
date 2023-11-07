
enable_language(C)

set(CMAKE_C_LINK_LIBRARIES_PROCESSING WRONG=REVERSE UNICITY=ALL)

add_library(lib STATIC lib.c)
