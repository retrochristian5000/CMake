cmake-default-dir-install-permissions
-------------------------------------

* The :variable:`CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS` variable was added
  to enable setting of default permissions for directories that are implicitly
  created by the calls to :command:`install` and :command:`file(INSTALL)`
  commands.

* The :variable:`CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS` variable was added
  which serves the same purpose during packaging as the
  :variable:`CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS` variable during
  invocation of `cmake install`.
