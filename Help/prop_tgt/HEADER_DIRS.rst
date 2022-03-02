HEADER_DIRS
-----------

.. versionadded:: 3.23

Semicolon-separated list of base directories of the target's default
file set, which is of type ``HEADERS``. The property supports
:manual:`generator expressions <cmake-generator-expressions(7)>`.

This property is normally only set by :command:`target_sources(FILE_SET)`
rather than being manipulated directly.

See :prop_tgt:`HEADER_DIRS_<NAME>` for the list of base directories in
other file sets.
