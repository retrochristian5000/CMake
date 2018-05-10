
add_link_options($<$<AND:$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>,$<CONFIG:${TEST_CONFIG}>>:${pre}BADFLAG_SHARED_$<UPPER_CASE:${TEST_CONFIG}>${obj}>)
add_link_options($<$<AND:$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>,$<CONFIG:${TEST_CONFIG}>>:${pre}BADFLAG_MODULE_$<UPPER_CASE:${TEST_CONFIG}>${obj}>)
add_link_options($<$<AND:$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>,$<CONFIG:${TEST_CONFIG}>>:${pre}BADFLAG_EXECUTABLE_$<UPPER_CASE:${TEST_CONFIG}>${obj}>)

add_library(LinkOptions_dll_flags_config SHARED ../LinkOptionsLib.c)

add_library(LinkOptions_mod_flags_config MODULE ../LinkOptionsLib.c)

add_executable(LinkOptions_exe_flags_config ../LinkOptionsExe.c)
