target_headers
--------------

Add headers to a target.

.. code-block:: cmake

  target_headers(<target>
    <INTERFACE|PUBLIC|PRIVATE> [HEADER_SET set1] [DIRECTORY dir1] [HEADERS headers1...]
    [<INTERFACE|PUBLIC|PRIVATE> [HEADER_SET set2] [DIRECTORY dir2] [HEADERS headers2...] ...])

Targets have zero or more named header sets and an optional anonymous header
set. Each header set has a name, a scope of ``INTERFACE``, ``PUBLIC``, or
``PRIVATE``, a base directory, and header files within that directory. The
optional anonymous header set always has a scope of either ``INTERFACE`` or
``PUBLIC``, and cannot be ``PRIVATE``.

Headers in a ``PRIVATE`` or ``PUBLIC`` header set (including the anonymous
header set if it's marked ``PUBLIC``) are marked as source files for the
purposes of IDE integration, and have their :prop_sf:`HEADER_FILE_ONLY`
property set to ``TRUE``. Headers in an ``INTERFACE`` or ``PUBLIC`` header set
(including the anonymous header set) can be installed with the
:command:`install(TARGETS)` command, and exported with the
:command:`install(EXPORT)` and :command:`export` commands.

Each ``target_headers()`` entry starts with ``INTERFACE``, ``PUBLIC``, or
``PRIVATE`` and accepts the following arguments:

``HEADER_SET <set>``

  An optional string representing the name of the header set to create or add
  to. If this is not provided, the anonymous header set is used.

``DIRECTORY <dir>``

  An optional string representing the base directory of the header set. If this
  is not provided, :variable:`CMAKE_CURRENT_SOURCE_DIR` is used. This argument
  supports :manual:`generator expressions <cmake-generator-expressions(7)>`.

``HEADERS <headers>``

  An optional list of strings representing header files within the
  ``DIRECTORY``. This argument supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`. Header
  file arguments must not be absolute files, and must be relative to the
  ``DIRECTORY``.

The following target properties are set by ``target_headers()``:

:prop_tgt:`HEADER_SETS`

  List of ``PRIVATE`` and ``PUBLIC`` header sets associated with a target.
  Headers listed in these header sets are treated as source files for the
  purposes of IDE integration, and have their :prop_sf:`HEADER_FILE_ONLY`
  property set to ``TRUE``.

:prop_tgt:`INTERFACE_HEADER_SETS`

  List of ``INTERFACE`` and ``PUBLIC`` header sets associated with a target.
  Headers listed in these header sets can be installed with
  :command:`install(TARGETS)` and exported with :command:`install(EXPORT)` and
  :command:`export`.

:prop_tgt:`HEADER_SET`

  Headers in the anonymous header set associated with a target. This property
  supports :manual:`generator expressions <cmake-generator-expressions(7)>`.

:prop_tgt:`HEADER_SET_<NAME>`

  Headers in the named header set ``<NAME>`` associated with a target. This
  property supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`.

:prop_tgt:`HEADER_DIR`

  Base directory of the anonymous header set associated with a target. This
  property supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`.

:prop_tgt:`HEADER_DIR_<NAME>`

  Base directory of the header set ``<NAME>`` associated with a target. This
  property supports
  :manual:`generator expressions <cmake-generator-expressions(7)>`.
