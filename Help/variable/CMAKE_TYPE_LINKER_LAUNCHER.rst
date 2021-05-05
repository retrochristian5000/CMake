CMAKE_<TYPE>_LINKER_LAUNCHER
----------------------------

.. versionadded:: 3.21

Default value for :prop_tgt:`<TYPE>_LINKER_LAUNCHER` target property. This
variable is used to initialize the property on each target as it is created.
This is done only when ``<TYPE>`` is ``EXE``, ``MODULE``, ``SHARED``, or
``STATIC``.

This variable is initialized to the :envvar:`CMAKE_<TYPE>_LINKER_LAUNCHER`
environment variable if it is set.
