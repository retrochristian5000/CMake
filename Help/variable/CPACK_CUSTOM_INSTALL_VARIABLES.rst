CPACK_CUSTOM_INSTALL_VARIABLES
------------------------------

CPack variables (set via e.g. `cpack -D`,
`CPackConfig.cmake` or :variable:`CPACK_PROJECT_CONFIG_FILE` scripts) are not
directly visible by installation scripts (`cmake_install.cmake`) that cmake
generates for :command:`install()` invocations.

The `CPACK_CUSTOM_INSTALL_VARIABLES` variable allows you to communicate a list
of `key=value` pairs of variables to be set for when CPack invokes
`cmake_install.cmake` scripts.

For empty variable values the `=` may be omitted.

This allows to influence custom install script code
at CPack runtime without having to regenerate the install scripts.

Example
"""""""

  .. code-block:: cmake

    install(FILES large.txt DESTINATION data)

    install(CODE [[
      if(ENABLE_COMPRESSION)
        # "run-compressor" is a fictional tool that produces
        # large.txt.xz from large.txt and then removes the input file
        execute_process(COMMAND run-compressor $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/large.txt)
      endif()
    ]])

  `cpack` will now by default run the installation script
  with `ENABLE_COMPRESSION` unset resulting in
  a package containing the uncompressed `large.txt`.

  When invoked via e.g.

  .. code-block:: shell

    cpack -D "CPACK_CUSTOM_INSTALL_VARIABLES=ENABLE_COMPRESSION=1"

  the installation script will run with `ENABLE_COMPRESSION` set to `1`
  resulting in a package containing the compressed `large.txt.xz` instead.
