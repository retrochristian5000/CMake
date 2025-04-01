unset_variable
--------------

.. versionadded:: 4.1

Unset a variable, cache variable, or environment variable.

.. include:: UNSET_NOTE.txt

Unset Normal Variable
^^^^^^^^^^^^^^^^^^^^^

.. signature::
  unset_variable(<variable> [SCOPE (LOCAL|PARENT)+])

Removes a normal variable from the current or/and parent scope, causing it
to become undefined. The options are:

  ``SCOPE <scope>...``
    Specify which scopes are concerned.

    ``LOCAL``
      Remove the ``<variable>`` in the current function or directory scope.

    ``PARENT``
      Remove the ``<variable>`` in the scope above the current scope.
      See the same option in the :command:`set_variable` command for further
      details.

Unset Cache Entry
^^^^^^^^^^^^^^^^^

.. signature::
  unset_variable(CACHE{<variable>})

Removes a cache variable.

Unset Environment Variable
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. signature::
  unset_variable(ENV{<variable>})

Removes ``<variable>`` from the currently available
:manual:`Environment Variables <cmake-env-variables(7)>`.
Subsequent calls of ``$ENV{<variable>}`` will return the empty string.

This command affects only the current CMake process, not the process
from which CMake was called, nor the system environment at large,
nor the environment of subsequent build or test processes.

See Also
^^^^^^^^

* :command:`set_variable`
