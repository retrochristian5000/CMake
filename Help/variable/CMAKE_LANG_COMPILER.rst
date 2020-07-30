CMAKE_<LANG>_COMPILER
---------------------

The compiler for ``LANG``.

This is the command that will be used as the ``<LANG>`` compiler.  Once
set, you can not change this variable.

If compiler is not a full path to a compiler then CMake will search for the compiler.
If found then CMake will continue to use this compiler.

.. note::
  Options that are required to make the compiler work correctly can be included
  as items in a list; they can not be changed.

.. code-block:: cmake

  set(CMAKE_C_COMPILER qcc --arg1 --arg2)

or

.. code-block:: console

  $ cmake ... -DCMAKE_C_COMPILER='qcc;--arg1;--arg2'
