set_source_files_properties
---------------------------

Source files can have properties that affect how they are built.

.. code-block:: cmake

  set_source_files_properties(<file1> ...
                              [DIRECTORY <dir1> ...]
                              [TARGET_DIRECTORY <target1> ...]
                              PROPERTIES <prop1> <value1>
                              [<prop2> <value2>] ...)

Sets properties associated with source files using a key/value paired
list.

By default, source file properties are only visible to targets added in the
same directory (``CMakeLists.txt``).  Visibility can be set in other directory
scopes using one or both of the options ``DIRECTORY`` or ``TARGET_DIRECTORY``:

  ``DIRECTORY`` takes a list of directories and sets the source file properties
  in those directory scopes.  CMake must already know about each of those
  source directories, either by having added them through a call to
  :command:`add_subdirectory` or it being the top level source directory.
  Relative paths are treated as relative to the current source directory.

  ``TARGET_DIRECTORY`` takes a list of existing targets. The source file
  properties will be set in each of the directory scopes where any of these
  targets were created.

See also the :command:`set_property(SOURCE)` command.

See :ref:`Source File Properties` for the list of properties known
to CMake.
