CMAKE_COMPILER_COLOR_DIAGNOSTICS
--------------------------------

Enable/disable colored diagnostics for compilers.

This variable uses three states: ``ON``, ``OFF`` and not defined.
When ``ON`` will force GNU/Clang compilers to use colored diagnostics
(``-fcolor-diagnostics``). When ``OFF`` will force GNU/Clang compilers
to disable colored diagnostics (``-fno-color-diagnostics``). When not
defined nothing additional is passed.

Defaults to ``ON`` for :ref:`Ninja Generators` and not defined otherwise.
