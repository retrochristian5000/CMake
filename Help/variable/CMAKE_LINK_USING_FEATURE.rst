CMAKE_LINK_USING_<FEATURE>
--------------------------

.. versionadded:: 3.23

This variable defines, for the specified ``FEATURE``, the expression expected
by the linker, regardless the linker language, when libraries are specified
using :genex:`LINK_LIBRARY` generator expression.

See also the associated variable
:variable:`CMAKE_LINK_USING_<FEATURE>_SUPPORTED`.

This variable will be used by :genex:`LINK_LIBRARY` generator expression if,
for the linker language, the variable
:variable:`CMAKE_<LANG>_LINK_USING_<FEATURE>_SUPPORTED` is false.

See :variable:`CMAKE_<LANG>_LINK_USING_<FEATURE>` for a detailed description of
its usage and capabilities.

.. include:: LINK_LIBRARY_PREDEFINED_FEATURES.txt
