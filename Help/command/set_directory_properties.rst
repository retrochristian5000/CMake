set_directory_properties
------------------------

Sets properties of the current directory and subdirectories in key-value pairs.

::

  set_directory_properties(PROPERTIES prop1 value1 prop2 value2)

See :ref:`Directory Properties` for the list of properties known to CMake.

Note that for changes to :prop_dir:`INCLUDE_DIRECTORIES` to be applied to
targets in the current directory, ``set_directory_properties`` must be called
prior to invoking :command:`add_executable` or :command:`add_library`.
