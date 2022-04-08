CXX_MODULE_HEADER_SETS
----------------------

.. versionadded:: 3.24

Read-only list of the target's ``PRIVATE`` and ``PUBLIC`` C++ module header
sets (i.e. all file sets with the type ``CXX_MODULE_HEADERS``). Files listed
in these file sets are treated as source files for the purpose of IDE
integration. The files also have their :prop_sf:`CXX_MODULE_HEADER_FILE_ONLY`
property set to ``TRUE``.

C++ module header sets may be defined using the :command:`target_sources`
command ``FILE_SET`` option with type ``CXX_MODULE_HEADERS``.

See also :prop_tgt:`CXX_MODULE_HEADER_SET_<NAME>`,
:prop_tgt:`CXX_MODULE_HEADER_SET` and
:prop_tgt:`INTERFACE_CXX_MODULE_HEADER_SETS`.
