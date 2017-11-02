CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
-------------------------------------------

Default permissions for implicitly created directories during packaging.

This variable serves the same purpose during packaging as the
:variable:`CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS` variable during
invocation of `cmake install`.

If `include(CPack)` is used then by default this variable is set to the content
of :variable:`CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS`.
