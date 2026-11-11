<PROJECT-NAME>_USE_SPDX_LICENSE_FOR_TARGETS
-------------------------------------------

.. versionadded:: 4.6

``ON`` if the most recent call to the :command:`project` command with project
name ``<PROJECT-NAME>`` was given the ``USE_SPDX_LICENSE_FOR_TARGETS`` option,
``OFF`` otherwise.

When true, the :prop_tgt:`SPDX_LICENSE` property of each non-imported target
created while ``<PROJECT-NAME>`` is the current project is initialized to the
value of :variable:`<PROJECT-NAME>_SPDX_LICENSE`.

The :command:`project` code injection hooks of
:variable:`CMAKE_PROJECT_INCLUDE` and
:variable:`CMAKE_PROJECT_<PROJECT-NAME>_INCLUDE` may set
``<PROJECT-NAME>_SPDX_LICENSE`` and this variable to ``ON`` to opt in a
project without modifying its ``project()`` call.
