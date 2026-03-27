enable_language(C)

add_library(lib1 STATIC)
target_sources(lib1 PUBLIC FILE_SET a TYPE SOURCES FILES lib1.c)

set_property(FILE_SET a TARGET lib1 PROPERTY COMPILE_OPTIONS -DOPT_FS)

set_property(TARGET lib1 PROPERTY COMPILE_OPTIONS -DOPT_TGT)
set_property(SOURCE lib1.c PROPERTY COMPILE_OPTIONS -DOPT_SRC)
