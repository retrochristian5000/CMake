SKIP_AUTOUIC
------------

Exclude the source file from :prop_tgt:`AUTOUIC` processing (for Qt projects).

For broader exclusion control see :prop_sf:`SKIP_AUTOGEN`.

EXAMPLE
^^^^^^^

.. code-block:: cmake

  # ...
  set_property(SOURCE file.h PROPERTY SKIP_AUTOUIC ON)
  set_source_files_properties(file1.h file2.h PROPERTIES SKIP_AUTOUIC ON)
  # ...
