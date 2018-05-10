
add_library(example SHARED empty.c)
target_link_options(example PRIVATE "LINKER:-foo,SHELL:-bar")
