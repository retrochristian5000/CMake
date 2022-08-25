CMAKE_EXPORT_COMPILE_COMMANDS
-----------------------------

.. versionadded:: 3.17

.. include:: ENV_VAR.txt

The default value for :variable:`CMAKE_EXPORT_COMPILE_COMMANDS` when there
is no explicit configuration given on the first run while creating a new
build tree.  On later runs in an existing build tree the value persists in
the cache as :variable:`CMAKE_EXPORT_COMPILE_COMMANDS`.

.. note::
  This environment variable is only honored by :ref:`Makefile Generators`
  and :ref:`Ninja Generators`.  It is ignored by other generators.
