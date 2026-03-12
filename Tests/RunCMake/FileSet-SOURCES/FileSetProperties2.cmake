enable_language(C)

add_library(lib1 STATIC)
target_sources(lib1 PRIVATE lib1.c)
target_sources(lib1 PUBLIC FILE_SET a TYPE SOURCES FILES lib4.c)

set_property(FILE_SET a TARGET lib1 PROPERTY COMPILE_OPTIONS -DOPT_FS)
set_property(FILE_SET a TARGET lib1 PROPERTY INTERFACE_COMPILE_OPTIONS -DOPT_INTERFACE_FS)

add_executable(main main.c)
target_link_libraries(main PRIVATE lib1)
