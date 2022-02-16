CMAKE_COLOR_DIAGNOSTICS
-----------------------

Enable colored diagnostics throughout.

This variable uses three states: ``ON``, ``OFF`` and not defined.
When ``ON``:

- will set :variable:`CMAKE_COLOR_MAKEFILE` to ``ON``,
- will enable colored diagnostics in GNU/Clang compilers
  (``-fcolor-diagnostics``).

When ``OFF``:

- will set :variable:`CMAKE_COLOR_MAKEFILE` to ``OFF``,
- will disable colored diagnostics in GNU/Clang compilers
  (``-fno-color-diagnostics``).

When not defined the default behavior is used.

If the :envvar:`CMAKE_COLOR_DIAGNOSTICS` environment variable is set, its
value is used. Otherwise, ``CMAKE_COLOR_DIAGNOSTICS`` is not defined by
default.
