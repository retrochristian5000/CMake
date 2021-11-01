CMAKE_<LANG>_LINK_USING_<FEATURE>
---------------------------------

.. versionadded:: 3.23

This variable defines, for the specified ``<FEATURE>`` and the linker language
``<LANG>``, the expression expected by the linker when libraries are specified
using :genex:`LINK_LIBRARY` generator expression.

See also the associated variable
:variable:`CMAKE_<LANG>_LINK_USING_<FEATURE>_SUPPORTED`.

It can contain one or three elements.

.. code-block::

  [<PREFIX>] <LIBRARY_EXPRESSION> [<SUFFIX>]

When ``<PREFIX>`` and/or ``<SUFFIX>`` are specified, they encapsulate the list
of libraries.

.. note::

  Even if ``<PREFIX>`` and ``<SUFFIX>`` are specified, there is not guarantee
  that the list of specified libraries, as part of :genex:`LINK_LIBRARY`
  generator expression, will be kept grouped. So, constructs like
  ``start-group`` and ``end-group``, as supported by ``GNU ld``, cannot be
  used.

``<LIBRARY_EXPRESSION>`` is used to specify the decoration for each
library. For that purpose, the patterns ``<LIBRARY>`` and ``<LINK_ITEM>`` are
available:

* ``<LIBRARY>`` is expanded to the library as computed by
  ``CMake``.
* ``<LINK_ITEM>`` is equivalent to ``<LIBRARY>`` for ``CMake`` targets and is
  expanded to the item specified by the user for external libraries.

Moreover, if ``<LIBRARY_EXPRESSION>`` is an empty string, ``CMake`` will
generate, for each library, the same expression as if the libraries were
specified in the standard way.

For all three elements, the ``LINKER:`` prefix can be used.

.. include:: ../command/LINK_OPTIONS_LINKER.txt

Example
^^^^^^^

A common need is the capability to load a whole static library. This capability
is offered by various environments but with a specific syntax:

.. code-block:: cmake

  set(CMAKE_C_LINK_USING_WHOLE_ARCHIVE_SUPPORTED TRUE)
  if(CMAKE_C_COMPILER_ID STREQUAL Clang)
    set(CMAKE_C_LINK_USING_WHOLE_ARCHIVE "-force_load <LINK_ITEM>")
  elseif(CMAKE_C_COMPILER_ID STREQUAL GNU)
    set(CMAKE_C_LINK_USING_WHOLE_ARCHIVE "LINKER:--whole-archive" "" "LINKER:--no-whole-archive")
  elseif(CMAKE_C_COMPILER_ID STREQUAL MSVC)
    set(CMAKE_C_LINK_USING_WHOLE_ARCHIVE "/WHOLEARCHIVE:<LIBRARY>")
  else()
    # feature not yet supported for the other environments
    set(CMAKE_C_LINK_USING_WHOLE_ARCHIVE_SUPPORTED FALSE)
  endif()

  add_library(lib1 STATIC ...)

  add_library(lib3 ...)
  if(CMAKE_C_LINK_USING_WHOLE_ARCHIVE_SUPPORTED)
    target_link_libraries(lib3 PRIVATE
      $<LINK_LIBRARY:WHOLE_ARCHIVE,lib1,$<IF:$<LINK_LANG_AND_ID:C,Clang>,libexternal.a,external>>)
  else()
    target_link_libraries(lib3 PRIVATE lib1 external)
  endif()

``CMake`` will generate the following link expressions:

* ``Clang``: ``-force_load /path/to/lib1.a -force_load libexternal.a``
* ``GNU``: ``-Wl,--whole-archive /path/to/lib1.a -lexternal -Wl,--no-whole-archive``
* ``MSVC``: ``/WHOLEARCHIVE:/path/to/lib1.lib /WHOLEARCHIVE:external.lib``

``CMake`` will ensure, when possible, that ``<PREFIX>`` and ``<SUFFIX>`` are
not repeated for each library.

In case of ``Clang``, the pattern ``<LINK_ITEM>`` is used because we need to
specify the library as defined by the user, not the name computed by ``CMake``
(in that case ``external``).
