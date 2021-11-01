enable_language(C)

set(CMAKE_C_LINK_USING_FEAT1_SUPPORTED TRUE)
set(CMAKE_C_LINK_USING_FEAT1 "<LIBRARY>")

set(CMAKE_C_LINK_USING_FEAT2_SUPPORTED TRUE)
set(CMAKE_C_LINK_USING_FEAT2 "<LIBRARY>")

add_library(dep1 SHARED empty.c)

add_library(dep2 SHARED empty.c)
target_link_libraries(dep2 PRIVATE $<LINK_LIBRARY:FEAT1,dep1>)

add_library(lib SHARED empty.c)
target_link_libraries(lib PRIVATE $<LINK_LIBRARY:FEAT2,dep1,dep2>)
