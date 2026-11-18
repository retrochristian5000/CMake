target_sources
--------------

.. versionadded:: 3.1

Add sources to a target.

.. code-block:: cmake

  target_sources(<target>
    {INTERFACE|PUBLIC|PRIVATE} <source>...
    [{INTERFACE|PUBLIC|PRIVATE} <source>...]...)

Specifies sources to use when building a target and/or its dependents.
The named ``<target>`` must have been created by a command such as
:command:`add_executable` or :command:`add_library` or
:command:`add_custom_target`.  The ``<source>`` items may use
:manual:`generator expressions <cmake-generator-expressions(7)>`.

.. versionadded:: 3.20
  ``<target>`` can be a custom target.

.. versionchanged:: 4.5
  If ``<target>`` is an :ref:`Alias Target <Alias Targets>`, the command
  operates on the target which the alias references.

The ``INTERFACE``, ``PUBLIC`` and ``PRIVATE`` keywords are required to
specify the :ref:`scope <Target Command Scope>` of the source file paths
(``<source>...``) that follow them.  ``PRIVATE`` and ``PUBLIC`` sources will
populate the :prop_tgt:`SOURCES` property of ``<target>``, which are used when
building the target itself. ``PUBLIC`` and ``INTERFACE`` sources will populate
the :prop_tgt:`INTERFACE_SOURCES` property of ``<target>``, which are used
when building dependents.  A target created by :command:`add_custom_target`
can only have ``PRIVATE`` scope.

Repeated calls for the same ``<target>`` append sources in the order called.

.. versionadded:: 3.3
  Allow exporting targets with :prop_tgt:`INTERFACE_SOURCES`.

.. versionadded:: 3.11
  Allow setting ``INTERFACE`` sources on
  :ref:`IMPORTED targets <Imported Targets>`.

.. versionchanged:: 3.13
  Relative source file paths are interpreted as being relative to the current
  source directory (i.e. :variable:`CMAKE_CURRENT_SOURCE_DIR`).
  See policy :policy:`CMP0076`.

A path that begins with a generator expression is left unmodified.
When a target's :prop_tgt:`SOURCE_DIR` property differs from
:variable:`CMAKE_CURRENT_SOURCE_DIR`, use absolute paths in generator
expressions to ensure the sources are correctly assigned to the target.

.. code-block:: cmake

  # WRONG: starts with generator expression, but relative path used
  target_sources(MyTarget PRIVATE "$<$<CONFIG:Debug>:dbgsrc.cpp>")

  # CORRECT: absolute path used inside the generator expression
  target_sources(MyTarget PRIVATE "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/dbgsrc.cpp>")

See the :manual:`cmake-buildsystem(7)` manual for more on defining
buildsystem properties.

File Sets
^^^^^^^^^

.. versionadded:: 3.23

.. code-block:: cmake

  target_sources(<target>
    [{INTERFACE|PUBLIC|PRIVATE}
     [FILE_SET <set> [TYPE <type>] [BASE_DIRS <dirs>...] [FILES <files>...]]...
    ]...)

Adds a file set to a target, or adds files to an existing file set. Targets
have zero or more named file sets.

See the :manual:`cmake-buildsystem(7)` manual for more on :ref:`File Sets`,
including their types and scope semantics, properties, and installation/export
behavior.

Each ``target_sources(FILE_SET)`` entry starts with ``INTERFACE``, ``PUBLIC``,
or ``PRIVATE`` and accepts the following arguments:

``FILE_SET <set>``

  The name of the file set to create or add to. It must contain only letters,
  numbers and underscores. Names starting with a capital letter are reserved
  for built-in file sets predefined by CMake. The only predefined set names
  are those matching the acceptable types. All other set names must not start
  with a capital letter or
  underscore.

``TYPE <type>``

  Every file set is associated with a particular type of file. Only types
  listed under :ref:`File Sets` may be used and it is an error to specify
  anything else. As a special case, if the name of the file set is one of these
  types, the type does not need to be specified and the ``TYPE <type>``
  arguments can be omitted. For all other file set names, ``TYPE`` is required.

  .. versionadded:: 4.5
    ``SYCL`` may be specified as an alias for ``SOURCES``.  It initializes the
    file set's :prop_fs:`LANGUAGE` property to ``SYCL``.

    ``SYCL_HEADERS`` may be specified as an alias for ``HEADERS``.  It also
    initializes :prop_fs:`LANGUAGE` to ``SYCL``.  A target with such a file set
    in its own build specification, or reachable through linked interface
    properties, compiles all of its C++ sources as SYCL.

    The canonical spellings ``FILE_SET SYCL TYPE SOURCES`` and
    ``FILE_SET SYCL_HEADERS TYPE HEADERS`` are also accepted.

    Both aliases are experimental and gated by ``CMAKE_EXPERIMENTAL_SYCL``.

``BASE_DIRS <dirs>...``

  An optional list of base directories of the file set. Any relative path
  is treated as relative to the current source directory
  (i.e. :variable:`CMAKE_CURRENT_SOURCE_DIR`). If no ``BASE_DIRS`` are
  specified when the file set is first created, the value of
  :variable:`CMAKE_CURRENT_SOURCE_DIR` is added. This argument supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`.

  No two base directories for a file set may be sub-directories of each other.
  This requirement must be met across all base directories added to a file set,
  not just those within a single call to ``target_sources()``.

``FILES <files>...``

  An optional list of files to add to the file set. Each file must be in
  one of the base directories, or a subdirectory of one of the base
  directories. This argument supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`.

  If relative paths are specified, they are considered relative to
  :variable:`CMAKE_CURRENT_SOURCE_DIR` at the time ``target_sources()`` is
  called. An exception to this is a path starting with ``$<``. Such paths
  are treated as relative to the target's source directory after evaluation
  of generator expressions.

Generated Interoperability Headers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. versionadded:: 4.6

.. code-block:: cmake

   target_sources(<target>
     [{INTERFACE|PUBLIC|PRIVATE}
      [GENERATE_INTEROP_HEADER <lang> <filename> [BASE_DIR base_dir] [FILE_SET <set>]]...
     ]...)

Declare and generate a C/C++ interoperability header for the target.

Each ``target_sources(GENERATE_INTEROP_HEADER)`` entry starts with
``INTERFACE``, ``PUBLIC``, or ``PRIVATE``, depending on whether the generated
header should be part of the target interface, is internal to building the
target, or both. In addition to a scope, the signature accepts the following
arguments:

``<lang>``

  The language exporting the C/C++ interoperability header.

``<filename>``

  The filename of the generated header. The file should be located in the base
  directory or a subdirectory of the base directory. This argument
  supports :manual:`generator expressions <cmake-generator-expressions(7)>`.

  If a relative path is specified, the path is considered relative to
  :variable:`CMAKE_CURRENT_BINARY_DIR` at the time ``target_sources()`` is
  called, unless the path starts with ``$<``. Paths starting with a generator
  expression must evaluate to an absolute path.

``BASE_DIR <dir>``

  An optional base directory of the generated header. The directory is added to
  the target's include directories, and to the base directories of the
  ``FILE_SET`` if one is specified. The header is included using its path
  relative to ``BASE_DIR``. If a relative path is specified, the path is
  considered relative to :variable:`CMAKE_CURRENT_BINARY_DIR` at the time
  ``target_sources()`` is called, unless the path starts with ``$<``. Such paths
  must evaluate to an absolute path. If no ``BASE_DIR`` is specified, the value
  of :variable:`CMAKE_CURRENT_BINARY_DIR` is used. This argument supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`.

  The base directory is part of the build interface and is not included in the
  install interface. When added to a ``FILE_SET``, the header is installed
  relative to the base directory.

  When specifying a ``FILE_SET``, no two base directories of the file set may be
  sub-directories of each other. This requirement must be made across all base
  directories added to the file set, not just those within a single call to
  ``target_sources()``.

``FILE_SET <set>``

  An optional file set name to add the generated header to. If the file set does
  not exist, a new file set is created with the ``HEADERS`` type. The named file
  set must have the same visibility scope as the generated header.

.. Note::

   ``GENERATE_INTEROP_HEADER`` is only implemented for the ``Swift`` language.

.. Note::

   ``GENERATE_INTEROP_HEADER`` is implemented for the :ref:`Ninja generators`
   and the :generator:`Xcode` generator.

See Also
^^^^^^^^

* :command:`add_executable`
* :command:`add_library`
* :command:`target_compile_definitions`
* :command:`target_compile_features`
* :command:`target_compile_options`
* :command:`target_include_directories`
* :command:`target_link_libraries`
* :command:`target_link_directories`
* :command:`target_link_options`
* :command:`target_precompile_headers`
