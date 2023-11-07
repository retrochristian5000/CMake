
enable_language(C)

set(CMAKE_C_LINK_LIBRARIES_PROCESSING ORDER=WRONG UNICITY=ALL)

add_library(lib STATIC lib.c)
