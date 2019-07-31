CMAKE_LINK_PCH
--------------

The variable specifies if the usage of the precompiled header
specified by :prop_tgt:`PRECOMPILE_HEADERS` requires linking
to an object file.

See also :variable:`CMAKE_<LANG>_COMPILE_OPTIONS_CREATE_PCH`,
:variable:`CMAKE_<LANG>_COMPILE_OPTIONS_USE_PCH`, and
:variable:`CMAKE_PCH_EXTENSION`

.. code-block:: cmake

  # MSVC
  set(CMAKE_LINK_PCH ON)

  # GCC / Clang
  set(CMAKE_LINK_PCH OFF)
