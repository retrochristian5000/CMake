enable_language(C CXX)

set_property(SOURCE a.h PROPERTY LANGUAGE C)
set_property(SOURCE dir/c.h PROPERTY LANGUAGE C)
set_property(SOURCE dir/cxx.h PROPERTY LANGUAGE CXX)

add_library(static STATIC empty.c)
target_sources(static INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_library(shared SHARED empty.c)
target_sources(shared INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_library(object OBJECT empty.c)
target_sources(object INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_executable(exe main.c)
target_sources(exe INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_executable(export_exe main.c)
set_property(TARGET export_exe PROPERTY ENABLE_EXPORTS TRUE)
target_sources(export_exe INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_library(none STATIC empty.c)

add_library(property_off STATIC empty.c)
target_sources(property_off INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)
set_property(TARGET property_off PROPERTY VERIFY_HEADER_SETS OFF)

add_library(a_h STATIC empty.c)
target_compile_definitions(a_h INTERFACE TEST_A_H)
target_sources(a_h INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_library(dir_c_h STATIC empty.c)
target_compile_definitions(dir_c_h INTERFACE TEST_DIR_C_H)
target_sources(dir_c_h INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

add_library(dir_cxx_h STATIC empty.c)
target_compile_definitions(dir_cxx_h INTERFACE TEST_DIR_CXX_H)
target_sources(dir_cxx_h INTERFACE FILE_SET HEADERS FILES a.h dir/c.h dir/cxx.h)

set_property(SOURCE debug.h PROPERTY LANGUAGE C)
set_property(SOURCE release.h PROPERTY LANGUAGE C)

add_library(config STATIC empty.c)
target_sources(config INTERFACE FILE_SET HEADERS FILES $<IF:$<CONFIG:Debug>,debug.h,release.h>)
