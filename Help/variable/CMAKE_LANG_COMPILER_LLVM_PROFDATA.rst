CMAKE_<LANG>_COMPILER_LLVM_PROFDATA
-----------------------------------

.. versionadded:: 4.6

The ``llvm-profdata`` tool matching the Clang compiler.

It is searched next to the compiler, or else in the ``PATH`` with a name
including the version of the compiler, e.g. ``llvm-profdata-18``, as the
format of the profile data changes between versions.

It is used by the ``cmake_pgo_merge`` target to merge the profile data
collected for :prop_tgt:`PROFILE_GUIDED_OPTIMIZATION`.  Without it, the
``GENERATE`` phase is not supported.  It may be set to the location of the
tool if it is not found.
