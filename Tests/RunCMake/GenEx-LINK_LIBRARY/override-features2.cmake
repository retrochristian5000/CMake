
include(incompatible-features.cmake)

set_property(TARGET lib PROPERTY LINK_LIBRARY_OVERRIDE "FEAT2,dep1")
