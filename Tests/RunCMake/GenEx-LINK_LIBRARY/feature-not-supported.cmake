enable_language(C)

set(CMAKE_C_LINK_USING_FEAT_SUPPORTED FALSE)
set(CMAKE_C_LINK_USING_FEAT "<LIBRARY>")

add_library(dep SHARED empty.c)

add_library(lib SHARED empty.c)
target_link_libraries(lib PRIVATE $<LINK_LIBRARY:FEAT,dep>)
