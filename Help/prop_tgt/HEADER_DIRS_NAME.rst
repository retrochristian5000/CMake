HEADER_DIRS_<NAME>
------------------

.. versionadded:: 3.23

Semicolon-separated list of base directories of the target's ``<NAME>``
file set. The property supports
:manual:`generator expressions <cmake-generator-expressions(7)>`.

This property is normally only set by :command:`target_sources(FILE_SET)`
rather than being manipulated directly.

See :prop_tgt:`HEADER_DIRS` for the list of base directories in the
default file set. See :prop_tgt:`HEADER_SETS` for the file set names of all
file sets with the ``HEADERS`` type.
