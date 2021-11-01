enable_language(C)

link_directories($<LINK_LIBRARY:FEAT>)

add_library(empty SHARED empty.c)
