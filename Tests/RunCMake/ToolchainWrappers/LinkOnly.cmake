include(common.cmake)
enable_language(C)

# `$<LINK_ONLY:...>` dependencies contribute only their link usage
# requirements, so `iface`'s INTERFACE_LINK_WRAPPERS apply to `main`
# but its INTERFACE_COMPILE_WRAPPERS do not.
add_library(link_only INTERFACE)
target_link_libraries(link_only INTERFACE "$<LINK_ONLY:iface>")

add_executable(main main.c)
target_link_libraries(main PRIVATE link_only)
