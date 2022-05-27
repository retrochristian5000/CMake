INTERFACE_CXX_MODULE_INTERNAL_PARTITION_SETS
--------------------------------------------

.. note ::

  Experimental. Gated by ``CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API``

Read-only list of the target's ``PUBLIC`` C++ module internal partition sets
(i.e. all file sets with the type ``CXX_MODULE_INTERNAL_PARTITIONS``). Files
listed in these C++ module internal partition sets can be installed with
:command:`install(TARGETS)` and exported with :command:`install(EXPORT)` and
:command:`export`.

C++ module internal partition sets may be defined using the
:command:`target_sources` command ``FILE_SET`` option with type
``CXX_MODULE_INTERNAL_PARTITIONS``.

See also :prop_tgt:`CXX_MODULE_INTERNAL_PARTITION_SETS`.
