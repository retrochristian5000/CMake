
add_link_options($<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${pre}BADFLAG_SHARED${obj}>)
add_link_options($<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${pre}BADFLAG_MODULE${obj}>)
add_link_options($<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${pre}BADFLAG_EXECUTABLE${obj}>)

add_library(LinkOptions_dll_flags SHARED ../LinkOptionsLib.c)

add_library(LinkOptions_mod_flags MODULE ../LinkOptionsLib.c)

add_executable(LinkOptions_exe_flags ../LinkOptionsExe.c)
