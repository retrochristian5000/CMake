CMAKE_<LANG>_COMPILE_OPTIONS_USE_PCH
------------------------------------

Variable contains the compiler options needed for using the
precompiled header specified by :prop_tgt:`PRECOMPILE_HEADERS`.

See also :variable:`CMAKE_<LANG>_COMPILE_OPTIONS_CREATE_PCH`,
:variable:`CMAKE_PCH_EXTENSION`, and :variable:`CMAKE_LINK_PCH`

.. code-block:: cmake

  # MSVC
  set(CMAKE_CXX_COMPILE_OPTIONS_USE_PCH /Yu<PCH_HEADER> /FI<PCH_HEADER>)

  # GCC / Clang
  set(CMAKE_CXX_COMPILE_OPTIONS_USE_PCH -Winvalid-pch -include <PCH_HEADER>)
