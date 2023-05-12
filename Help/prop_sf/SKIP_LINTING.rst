SKIP_LINTING
------------

.. versionadded:: 3.27

Exclude the source file from `LINTING` process. The `LINTING` process
includes `cpplint`, `clang-tidy`, `cppcheck` and `include-what-you-use`.
IF `SKIP_LINTING` is set on a source file, the above mentioned tools will
not be run on that source file.

``SKIP_LINTING`` can be set on source files as on.

EXAMPLE
^^^^^^^

.. code-block:: cmake

  # ...
  set_property(SOURCE file.cpp PROPERTY SKIP_LINTING ON)
  # ...
