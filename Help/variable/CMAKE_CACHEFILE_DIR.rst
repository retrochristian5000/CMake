CMAKE_CACHEFILE_DIR
-------------------

The directory with the ``CMakeCache.txt`` file.

This is the full path to the directory that has the ``CMakeCache.txt``
file in it.  This is the same as :variable:`CMAKE_BINARY_DIR`.
However, during configuration, when the ``CMakeCache.txt`` file has not
been written yet, only :variable:`CMAKE_BINARY_DIR` might be defined.
