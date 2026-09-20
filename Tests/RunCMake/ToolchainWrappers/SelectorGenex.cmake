enable_language(C)
enable_language(CXX)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# {COMPILE,LINK}_WRAPPERS lists themselves may be selected with
# $<COMPILE_LANGUAGE:...>/$<LINK_LANGUAGE:...>.

set_property(GLOBAL PROPERTY
  CMAKE_COMPILE_WRAPPER_SELECTOR
    "${CMAKE_COMMAND};-E;env;CW_SELECTOR=1"
)
set_property(GLOBAL PROPERTY
  CMAKE_LINK_WRAPPER_SELECTOR
    "${CMAKE_COMMAND};-E;env;LW_SELECTOR=1"
)

add_executable(mainc main.c)
set_target_properties(mainc PROPERTIES
  COMPILE_WRAPPERS "$<$<COMPILE_LANGUAGE:CXX>:SELECTOR>"
  LINK_WRAPPERS "$<$<LINK_LANGUAGE:CXX>:SELECTOR>"
)

add_executable(maincxx main.cxx)
set_target_properties(maincxx PROPERTIES
  COMPILE_WRAPPERS "$<$<COMPILE_LANGUAGE:CXX>:SELECTOR>"
  LINK_WRAPPERS "$<$<LINK_LANGUAGE:CXX>:SELECTOR>"
)
