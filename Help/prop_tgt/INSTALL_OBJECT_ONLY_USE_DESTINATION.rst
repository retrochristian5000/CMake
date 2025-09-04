INSTALL_OBJECT_ONLY_USE_DESTINATION
-----------------------------------

.. versionadded:: 4.2

Controls whether the ``install(DESTINATION)`` for object libraries is used
as-is or supplemented with conflict-avoiding subdirectories.

When installing object files, CMake automatically adds
``objects[-<CONFIG>]/<TARGET_NAME>`` components to the destination to avoid
conflicts. Use this property to suppress these components.

This property is initialized by the value of
:variable:`CMAKE_INSTALL_OBJECT_ONLY_USE_DESTINATION` when the target is
created.
