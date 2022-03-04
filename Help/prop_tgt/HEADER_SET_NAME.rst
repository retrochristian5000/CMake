HEADER_SET_<NAME>
-----------------

.. versionadded:: 3.23

Semicolon-separated list of files in the target's ``<NAME>`` file set,
for files of type ``HEADERS``. If any of the paths are relative,
they are computed relative to the target's source directory. The property
supports :manual:`generator expressions <cmake-generator-expressions(7)>`.

This property is normally only set by :command:`target_sources(FILE_SET)`
rather than being manipulated directly.

See :prop_tgt:`HEADER_SET` for the list of files in the default file set.
See :prop_tgt:`HEADER_SETS` for the file set names of all file sets with
the ``HEADERS`` type.
