
include(incompatible-features1.cmake)


set(CMAKE_C_LINK_USING_FEAT3_SUPPORTED TRUE)
set(CMAKE_C_LINK_USING_FEAT3 "<LIBRARY>")

set_property(TARGET lib PROPERTY LINK_LIBRARY_OVERRIDE "FEAT1,dep1")
# next property will be ignored because no feature is specified
set_property(TARGET lib PROPERTY LINK_LIBRARY_OVERRIDE_dep1)
