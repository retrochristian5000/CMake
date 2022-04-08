CXX_MODULE_INTERNAL_PARTITION_DIRS_<NAME>
-----------------------------------------

.. note ::

  Experimental. Gated by ``CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API``

Semicolon-separated list of base directories of the target's ``<NAME>`` C++
module internal partition set, which has the set type
``CXX_MODULE_INTERNAL_PARTITIONS``. The property supports
:manual:`generator expressions <cmake-generator-expressions(7)>`.

This property is normally only set by :command:`target_sources(FILE_SET)`
rather than being manipulated directly.

See :prop_tgt:`CXX_MODULE_INTERNAL_PARTITION_DIRS` for the list of base
directories in the default C++ module internal partition set. See
:prop_tgt:`CXX_MODULE_INTERNAL_PARTITION_SETS` for the file set names of all
C++ module internal partition sets.
