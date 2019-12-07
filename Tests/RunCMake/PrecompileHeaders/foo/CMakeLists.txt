add_library(foo ../foo.c)
target_include_directories(foo PUBLIC ../include)
target_precompile_headers(foo REUSE_FROM empty)

# Visual Studio 2017 and greater
if (NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_LESS_EQUAL 19.10))
  set_target_properties(foo PROPERTIES PREFIX "lib" IMPORT_PREFIX "lib")
endif()
