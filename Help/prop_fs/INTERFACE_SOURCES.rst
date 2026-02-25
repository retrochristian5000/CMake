INTERFACE_SOURCES
-----------------

List of interface sources to compile into consuming targets.
The :command:`target_sources` command set or add to the ``INTERFACE_SOURCES``
file set property for the file sets defined with the ``PUBLIC`` or
``INTERFACE`` keyword and is the usual way to manipulate it.

Contents of ``INTERFACE_SOURCES`` may use
:manual:`generator expressions <cmake-generator-expressions(7)>`.

Each file must be in one of the base directories, or a subdirectory of one of
the base directories.

If relative paths are specified, they are considered relative to the target's
source directory.

Currently, :prop_fs:`SOURCES` and ``INTERFACE_SOURCES`` properties are
inter-dependents. Depending of the ``SCOPE`` of the file set, the following
behavior apply:

``PRIVATE``
  Only the :prop_fs:`SOURCES` property can be set. Any change to the
  ``INTERFACE_SOURCES`` property will be ignored.

``PUBLIC``
  :prop_fs:`SOURCES` and ``INTERFACE_SOURCES`` properties will always have the
  same content.

``INTERFACE``
  Only the ``INTERFACE_SOURCES`` property can be set. Any change to the
  :prop_fs:`SOURCES` property will be ignored.
