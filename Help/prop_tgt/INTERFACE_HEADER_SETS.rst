INTERFACE_HEADER_SETS
---------------------

.. versionadded:: 3.23

List of the target's ``INTERFACE`` and ``PUBLIC`` file sets of type
``HEADERS``. Files listed in these file sets can be installed with
:command:`install(TARGETS)` and exported with :command:`install(EXPORT)`
and :command:`export`.

This property is normally only set by :command:`target_sources(FILE_SET)`
rather than being manipulated directly.

See also :prop_tgt:`HEADER_SETS`.
