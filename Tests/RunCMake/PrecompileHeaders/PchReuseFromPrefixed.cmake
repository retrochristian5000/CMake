cmake_minimum_required(VERSION 3.15)
project(PchReuseFromPrefixed C)

if(CMAKE_C_COMPILE_OPTIONS_USE_PCH)
  add_definitions(-DHAVE_PCH_SUPPORT)
endif()

add_subdirectory(empty)
add_subdirectory(foo)

add_executable(foobar foobar.c)
target_link_libraries(foobar foo )
set_target_properties(foobar PROPERTIES PRECOMPILE_HEADERS_REUSE_FROM empty)

enable_testing()
add_test(NAME foobar COMMAND foobar)
