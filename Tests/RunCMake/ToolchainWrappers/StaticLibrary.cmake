include(common.cmake)
enable_language(C)

# Static libraries are created by the archiver: compile wrappers apply, but
# link wrappers and linker launchers do not.
set(CMAKE_C_LINKER_LAUNCHER "${CMAKE_COMMAND};-E;env;LL_LAUNCH=1")
add_library(main STATIC main.c)
set_target_properties(main PROPERTIES
  COMPILE_WRAPPERS "EXAMPLE_1;EXAMPLE_2"
  LINK_WRAPPERS "EXAMPLE_1;EXAMPLE_2"
)
