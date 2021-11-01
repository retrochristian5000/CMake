enable_language(C)

set(CMAKE_C_LINK_USING_FEAT_SUPPORTED TRUE)
set(CMAKE_C_LINK_USING_FEAT "")

add_library(dep SHARED empty.c)

add_library(lib SHARED empty.c)
target_link_libraries(lib PRIVATE $<LINK_LIBRARY:FEAT,dep>)
