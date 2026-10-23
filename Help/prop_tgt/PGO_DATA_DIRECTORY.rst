PGO_DATA_DIRECTORY
------------------

.. versionadded:: 4.6

Directory holding the profile data of a target for
:prop_tgt:`PROFILE_GUIDED_OPTIMIZATION`.

The instrumented binaries built in the ``GENERATE`` phase write their
profile data to this directory, and the ``USE`` phase reads it from there.
CMake creates the directory for the ``GENERATE`` phase.  For the ``USE``
phase, it may point to the profile data of another build tree.
A relative path is interpreted with respect to the top-level build
directory, :variable:`CMAKE_BINARY_DIR`.  The directory may be shared by
multiple targets.  Binaries linking libraries built with profile guided
optimization, without the :prop_tgt:`PROFILE_GUIDED_OPTIMIZATION` property
set on them, use the directory of these libraries.

With Clang, the profile data of a binary is written to a single directory,
so it is an error if a binary links objects built in the ``GENERATE`` phase
by targets using different directories, e.g. a static library and the
executable linking it.

If not set, the ``pgo`` directory in the top-level build directory,
:variable:`CMAKE_BINARY_DIR`, is used.  With
:prop_gbl:`multi-config generators <GENERATOR_IS_MULTI_CONFIG>`,
a subdirectory named after the configuration is used, e.g. ``pgo/Release``.

The value may use
:manual:`generator expressions <cmake-generator-expressions(7)>`, except
those depending on the language, e.g. :genex:`$<COMPILE_LANGUAGE>`, as the
directory is used to compile and link all languages of the target.

This property is initialized by the :variable:`CMAKE_PGO_DATA_DIRECTORY`
variable if it is set when a target is created.
