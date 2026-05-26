cache-var-help-from-docs
------------------------

* ``cmake``'s :option:`-D <cmake -D>` command-line option now consults
  a built-in documentation table for commonly used CMake cache
  variables (for example :variable:`CMAKE_BUILD_TYPE`,
  :variable:`CMAKE_INSTALL_PREFIX`, :variable:`CMAKE_TOOLCHAIN_FILE`).
  When a user writes ``-DCMAKE_COMPILE_WARNING_AS_ERROR=ON`` with no
  ``HELPSTRING``, the resulting ``CMakeCache.txt`` entry now carries
  a one-line description drawn from the same source as ``cmake-gui``
  and ``ccmake`` tooltips.  The cache entry's type and value are
  unchanged: bare ``-D`` entries remain ``:UNINITIALIZED=`` and an
  explicit ``-D <var>:TYPE=<value>`` is honored as before.
  Project-specific variables absent from the table retain the
  legacy ``No help, variable specified on the command line.``
  description.
