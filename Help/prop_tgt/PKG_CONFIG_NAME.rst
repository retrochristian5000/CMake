PKG_CONFIG_NAME
----------------

.. versionadded:: 3.22

Name of the target in the pkg-config metadata library.

This string identifies how the target is handled when exporting to pkg-config
format. For referenced targets when this property is set, the target is
added to the ``Requires`` list of the created :file:`.pc` file, otherwise, target
properties are collected as ``Cflags`` and ``Libs``.
