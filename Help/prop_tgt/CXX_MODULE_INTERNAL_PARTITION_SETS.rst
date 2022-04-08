CXX_MODULE_INTERNAL_PARTITION_SETS
----------------------------------

.. note ::

  Experimental. Gated by ``CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API``

Read-only list of the target's ``PRIVATE`` and ``PUBLIC`` C++ module internal
partition sets (i.e. all file sets with the type
``CXX_MODULE_INTERNAL_PARTITIONS``). Files listed in these file sets are
treated as source files for the purpose of IDE integration.

C++ module internal partition sets may be defined using the
:command:`target_sources` command ``FILE_SET`` option with type
``CXX_MODULE_INTERNAL_PARTITIONS``.

See also :prop_tgt:`CXX_MODULE_INTERNAL_PARTITION_SET_<NAME>`,
:prop_tgt:`CXX_MODULE_INTERNAL_PARTITION_SET` and
:prop_tgt:`INTERFACE_CXX_MODULE_INTERNAL_PARTITION_SETS`.
