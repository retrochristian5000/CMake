HEADER_SETS
-----------

.. versionadded:: 3.23

List of the target's ``PRIVATE`` and ``PUBLIC`` file sets of type
``HEADERS``. Files listed in these file sets are treated as source
files for the purpose of IDE integration. The files also have their
:prop_sf:`HEADER_FILE_ONLY` property set to ``TRUE``.

This property is normally only set by :command:`target_sources(FILE_SET)`
rather than being manipulated directly.

See also :prop_tgt:`HEADER_SET_<NAME>` and :prop_tgt:`HEADER_SET`.
