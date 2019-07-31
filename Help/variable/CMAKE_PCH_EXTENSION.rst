CMAKE_PCH_EXTENSION
-------------------

Variable contains the file extension for generated precompile header
specified by :prop_tgt:`PRECOMPILE_HEADERS`.

See also :variable:`CMAKE_<LANG>_COMPILE_OPTIONS_CREATE_PCH`,
:variable:`CMAKE_<LANG>_COMPILE_OPTIONS_USE_PCH`, and :variable:`CMAKE_LINK_PCH`

.. code-block:: cmake

  # MSVC
  set(CMAKE_PCH_EXTENSION .pch)

  # GCC / Clang
  set(CMAKE_PCH_EXTENSION .gch)
