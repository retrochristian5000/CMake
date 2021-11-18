CMAKE_COMPILER_COLOR_DIAGNOSTICS
--------------------

Enable/Disable colored diagnostics for compilers.

This variable uses three states: ``ON``, ``OFF`` and not defined.
When ``ON`` will force GNU/Clang compilers to use colored diagnostics (``-fcolor-diagnostics``).
When ``OFF`` will force GNU/Clang compilers to disable colored diagnostics (``-fno-color-diagnostics``).
When this variable not defined CMake will not pass anything additional to the compilers.

Default is ``ON`` for :ref:`Ninja Generators` and not defined in other cases.
