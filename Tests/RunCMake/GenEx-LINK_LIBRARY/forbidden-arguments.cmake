enable_language(C)

add_library(lib SHARED empty.c)
target_link_libraries(lib PRIVATE <LINK_LIBRARY:FEAT> foo </LINK_LIBRARY:FEAT>)
