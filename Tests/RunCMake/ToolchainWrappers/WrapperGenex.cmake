enable_language(C)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# The command line held by CMAKE_{COMPILE,LINK}_WRAPPER_<NAME> may itself use
# generator expressions such as $<COMPILE_LANGUAGE:...>/$<LINK_LANGUAGE:...>.

set_property(GLOBAL PROPERTY
  CMAKE_COMPILE_WRAPPER_GENEX
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_COMMAND};-E;env;CW_GENEX=1>"
)
set_property(GLOBAL PROPERTY
  CMAKE_LINK_WRAPPER_GENEX
    "$<$<LINK_LANGUAGE:C>:${CMAKE_COMMAND};-E;env;LW_GENEX=1>"
)

add_executable(main main.c)
set_target_properties(main PROPERTIES
  COMPILE_WRAPPERS GENEX
  LINK_WRAPPERS GENEX
)
