cmake_minimum_required(VERSION 3.15)
project(PchReuseFrom C)

add_library(empty empty.c)
target_precompile_headers(empty PRIVATE
  <stdio.h>
  <string.h>
)
target_include_directories(empty PUBLIC include)

add_library(foo foo.c)
target_include_directories(foo PUBLIC include)
target_precompile_headers(foo REUSE_FROM empty)

# should not cause problems if configured multiple times
target_precompile_headers(foo REUSE_FROM empty)

add_executable(foobar foobar.c)
target_link_libraries(foobar foo )
set_target_properties(foobar PROPERTIES PRECOMPILE_HEADERS_REUSE_FROM foo)

if (CMAKE_C_COMPILE_OPTIONS_USE_PCH)
  target_compile_definitions(foobar PRIVATE HAVE_PCH_SUPPORT)
endif()

enable_testing()
add_test(NAME foobar COMMAND foobar)
