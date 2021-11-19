enable_language(C)

set(CMAKE_C_LINK_USING_FEAT1_SUPPORTED TRUE)
set(CMAKE_C_LINK_USING_FEAT1 "<LIBRARY>")

set(CMAKE_C_LINK_USING_FEAT2_SUPPORTED TRUE)
set(CMAKE_C_LINK_USING_FEAT2 "<LIBRARY>")

add_library(dep1 SHARED empty.c)

add_library(dep2 SHARED empty.c)
target_link_libraries(dep2 PRIVATE $<LINK_LIBRARY:FEAT1,dep1>)

add_library(dep3 SHARED empty.c)
target_link_libraries(dep3 PUBLIC dep2)

add_library(lib1 SHARED empty.c)
target_link_libraries(lib1 PRIVATE $<LINK_LIBRARY:FEAT2,dep1,dep2>)

add_library(lib2 SHARED empty.c)
target_link_libraries(lib2 PRIVATE $<LINK_LIBRARY:STD,dep2,dep3>)
