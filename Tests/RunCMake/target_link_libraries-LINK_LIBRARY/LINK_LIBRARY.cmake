
enable_language(C)

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES FALSE)

add_library(base1 SHARED base.c)
add_library(base2 SHARED base.c)

set(CMAKE_C_LINK_USING_FEAT1 "--LIBFLAG<LIBRARY>")
set(CMAKE_C_LINK_USING_FEAT1_SUPPORTED TRUE)

set(CMAKE_C_LINK_USING_FEAT1_1 "--LIBFLAG_C<LIBRARY>")
set(CMAKE_C_LINK_USING_FEAT1_1_SUPPORTED FALSE)
set(CMAKE_LINK_USING_FEAT1_1 "--LIBFLAG<LIBRARY>")
set(CMAKE_LINK_USING_FEAT1_1_SUPPORTED TRUE)

set(CMAKE_C_LINK_USING_FEAT2 "--PREFIXGROUP" "--LIBGROUP<LIBRARY>" "--SUFFIXGROUP")
set(CMAKE_C_LINK_USING_FEAT2_SUPPORTED TRUE)

set(CMAKE_C_LINK_USING_FEAT3 "--PREFIXGROUP" "<LINK_ITEM>" "--SUFFIXGROUP")
set(CMAKE_C_LINK_USING_FEAT3_SUPPORTED TRUE)

set(CMAKE_C_LINK_USING_FEAT4 "--PREFIXGROUP" "--LIBFLAG<LIBRARY> --ITEMFLAG<LIB_ITEM>" "--SUFFIXGROUP")
set(CMAKE_C_LINK_USING_FEAT4_SUPPORTED TRUE)

set(CMAKE_C_LINK_USING_FEAT5 "<LINK_ITEM>")
set(CMAKE_C_LINK_USING_FEAT5_SUPPORTED TRUE)


add_library(LinkLibrary_simple1 SHARED lib.c)
target_link_libraries(LinkLibrary_simple1 PRIVATE $<LINK_LIBRARY:FEAT1,base1>)

add_library(LinkLibrary_simple2 SHARED lib.c)
target_link_libraries(LinkLibrary_simple2 PRIVATE $<LINK_LIBRARY:FEAT1_1,base1>)

add_library(LinkLibrary_group1 SHARED lib.c)
target_link_libraries(LinkLibrary_group1 PRIVATE $<LINK_LIBRARY:FEAT1,base1,base2>)

add_library(LinkLibrary_group2 SHARED lib.c)
target_link_libraries(LinkLibrary_group2 PRIVATE $<LINK_LIBRARY:FEAT2,base1,base2>)

add_library(LinkLibrary_nested_feature1 SHARED lib.c)
target_link_libraries(LinkLibrary_nested_feature1 PRIVATE $<LINK_LIBRARY:FEAT1,base1,$<LINK_LIBRARY:FEAT1,base2>>)

add_library(LinkLibrary_nested_feature2 SHARED lib.c)
target_link_libraries(LinkLibrary_nested_feature2 PRIVATE $<LINK_LIBRARY:FEAT2,base1,$<LINK_LIBRARY:FEAT2,base2>>)

add_library(LinkLibrary_link_items1 SHARED lib.c)
target_link_libraries(LinkLibrary_link_items1 PRIVATE $<LINK_LIBRARY:FEAT3,base1,other>)

add_library(LinkLibrary_link_items2 SHARED lib.c)
target_link_libraries(LinkLibrary_link_items2 PRIVATE $<LINK_LIBRARY:FEAT2,base1,other>)

add_library(LinkLibrary_link_items3 SHARED lib.c)
target_link_libraries(LinkLibrary_link_items3 PRIVATE $<LINK_LIBRARY:FEAT4,base1,other>)

add_library(base3 SHARED base.c)
target_link_libraries(base3 PRIVATE $<LINK_LIBRARY:FEAT5,base1>)
add_library(LinkLibrary_mix_features1 SHARED lib.c)
target_link_libraries(LinkLibrary_mix_features1 PRIVATE $<LINK_LIBRARY:FEAT2,base1,base3>)

target_link_libraries(base3 INTERFACE $<LINK_LIBRARY:FEAT2,base1>)
add_library(LinkLibrary_mix_features2 SHARED lib.c)
target_link_libraries(LinkLibrary_mix_features2 PRIVATE $<LINK_LIBRARY:FEAT2,base1,base3>)

target_link_libraries(base3 INTERFACE other1)
add_library(LinkLibrary_mix_features3 SHARED lib.c)
target_link_libraries(LinkLibrary_mix_features3 PRIVATE base2 $<LINK_LIBRARY:FEAT2,base1,base3> other2)
