AIX_SHARED_LIBRARY_ARCHIVE
--------------------------

.. versionadded:: 3.31

On AIX, enable creation of a shared library archive.  This places
the shared object ``.so`` file inside an archive ``.a`` file.

By default, CMake creates shared libraries on AIX as plain
shared object ``.so`` files for consistency with other UNIX platforms.
Alternatively, set this property to a true value to create a shared
library archive instead, as is AIX convention.

When a shared library is archived the shared object in the archive
we record version information from :prop_tgt:`VERSION` or
:prop_tgt`SOVERSION` target properties with `SOVERSION` as the
highest priority for the shared object name.

This property defaults to :variable:`CMAKE_AIX_SHARED_LIBRARY_ARCHIVE`
if that variable is set when a ``SHARED`` library target is created
by :command:`add_library`.
