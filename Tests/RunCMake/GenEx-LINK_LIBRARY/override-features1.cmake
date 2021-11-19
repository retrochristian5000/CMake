
include(incompatible-features.cmake)

set_property(TARGET lib PROPERTY OVERRIDE_LINK_LIBRARY "FEAT1,dep1")
