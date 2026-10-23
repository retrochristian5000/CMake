PROFILE_GUIDED_OPTIMIZATION
---------------------------

.. versionadded:: 4.6

Enable profile guided optimization (PGO) for a target.

Profile guided optimization is a two phase process:

``GENERATE``
  The target is built with instrumentation.  Running the instrumented
  binaries with a representative workload writes profile data to the
  :prop_tgt:`PGO_DATA_DIRECTORY`.

``USE``
  The target is rebuilt using the profile data collected in the
  ``GENERATE`` phase to guide optimizations.

If set to a false value, or not set, profile guided optimization is not
enabled.  Any other value is an error.  It is also an error to enable
profile guided optimization with a compiler for which CMake does not know
how to do it.

The value may use
:manual:`generator expressions <cmake-generator-expressions(7)>`, except
those depending on the language, e.g. :genex:`$<COMPILE_LANGUAGE>`, as the
same mode is used to compile and link all languages of the target.  For
example, to enable profile guided optimization only for the ``Release``
configuration:

.. code-block:: cmake

  set_property(TARGET foo PROPERTY PROFILE_GUIDED_OPTIMIZATION
    "$<$<CONFIG:Release>:GENERATE>")

A typical workflow switches the value of the
:variable:`CMAKE_PROFILE_GUIDED_OPTIMIZATION` variable between the phases
in the same build tree:

.. code-block:: shell

  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PROFILE_GUIDED_OPTIMIZATION=GENERATE
  cmake --build build
  # Run the training workload, e.g.:
  ctest --test-dir build
  # Clang and MSVC only: merge the profile data
  cmake --build build --target cmake_pgo_merge
  cmake -DCMAKE_PROFILE_GUIDED_OPTIMIZATION=USE build
  cmake --build build

The build system does not know that the binaries built in the ``USE``
phase depend on the profile data.  After collecting new profile data,
rebuild them from scratch, e.g. using
:option:`cmake --build --clean-first <cmake--build --clean-first>`.

With the GNU and Clang compilers, the ``USE`` phase may also happen in a
separate build tree, by pointing its :prop_tgt:`PGO_DATA_DIRECTORY` to
the profile data collected by the ``GENERATE`` build tree:

.. code-block:: shell

  cmake -S . -B build-pgo -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PROFILE_GUIDED_OPTIMIZATION=USE \
        -DCMAKE_PGO_DATA_DIRECTORY=$PWD/build/pgo

CMake currently supports profile guided optimization for the ``C``,
``CXX``, ``OBJC``, ``OBJCXX``, and ``Fortran`` languages with the
following compilers:

``GNU``
  The ``-fprofile-generate=<dir>`` and ``-fprofile-use=<dir>`` flags are
  used.  The profile data files are named after the object files.  With
  GCC 11 and later, ``-fprofile-prefix-path=<dir>`` is used to make these
  names relative to the top-level build directory, so the profile data can
  be used by another build tree with the same layout.  This is not done if
  the path of the top-level build directory contains symbolic links, as
  GCC may see either path.  Otherwise, the ``USE`` phase must happen in the
  same build tree as the ``GENERATE`` phase.

``Clang``, ``AppleClang``
  Clang 3.9 and later, and AppleClang 8.0 and later.  The
  ``-fprofile-generate=<dir>`` and ``-fprofile-use=<dir>`` flags are used.
  For ``Fortran``, this applies to the classic ``Flang`` compiler, but not
  to ``LLVMFlang``.  Clang does not use the raw ``.profraw`` files written
  by the instrumented binaries directly.  Before the ``USE`` phase, they
  must be merged into a ``default.profdata`` file in the profile data
  directory.
  CMake generates a ``cmake_pgo_merge`` target doing that for the profile
  data directories of all targets, using the ``llvm-profdata`` tool, see
  :variable:`CMAKE_<LANG>_COMPILER_LLVM_PROFDATA`.  The ``GENERATE`` phase
  is not supported if that tool is not found.  For example:

  .. code-block:: shell

    cmake --build build --target cmake_pgo_merge

  The existing ``default.profdata`` file is merged too, so that it
  accumulates the profile data of all training runs.  Remove it to start
  over, e.g. after changing the sources.  The merged ``.profraw`` files are
  removed, so that stale raw profile data written by previous builds of the
  instrumented binaries is not merged again.  Running ``cmake_pgo_merge``
  without new raw profile data keeps the existing ``default.profdata``
  file, which may also come from elsewhere, e.g. another build tree.

  This also applies to Clang targeting the MSVC ABI, with either the
  GNU-like or the MSVC-like (``clang-cl``) command-line interface.  The
  instrumented binaries may be linked with either ``lld-link`` or MSVC's
  ``link.exe``, which needs ``/OPT:NOREF`` to keep the profile data, so
  CMake adds it.  With ``clang-cl``, CMake also adds the location of the
  Clang profiling runtime library, and the ``GENERATE`` phase is not
  supported if CMake cannot determine that location.

``MSVC``
  Visual Studio 2015 and later.  Sources are compiled with ``/GL`` and
  binaries are linked with ``/LTCG`` and ``/GENPROFILE:PGD=<file>`` or
  ``/USEPROFILE:PGD=<file>``, using a ``<target-name>.pgd`` file in the
  profile data directory.  The instrumented binaries write
  ``<target-name>!<n>.pgc`` files next to themselves, but the linker only
  uses those next to the ``.pgd`` file.  Before the ``USE`` phase, the
  ``cmake_pgo_merge`` target merges them into the ``.pgd`` files of all
  binaries linked in the ``GENERATE`` phase using the ``pgomgr`` tool, and
  removes them.  The ``.pgd`` files accumulate the profile data of all
  training runs until the binaries are linked again.  Running
  ``cmake_pgo_merge`` without new ``.pgc`` files keeps the merged profile
  data.  The ``GENERATE`` phase is not supported if ``pgomgr`` is not
  found next to the compiler.  The instrumented binaries need the
  ``pgort*.dll`` runtime library from the compiler toolset to run.
  Static libraries are archived with ``/LTCG`` if they hold objects built
  with profile guided optimization, including those of object libraries.
  Only the sources of targets with profile guided optimization enabled are
  compiled with ``/GL``, also with :ref:`Visual Studio Generators`.

This property is initialized by the
:variable:`CMAKE_PROFILE_GUIDED_OPTIMIZATION` variable if it is set when a
target is created.

Executables, shared libraries, and module libraries linking static or
object libraries built with profile guided optimization, or using the
objects of such object libraries in their sources, are linked with the
profile guided optimization options, even if the property is not set on
them, e.g. to link the profiling runtime library in the ``GENERATE``
phase.  If any of these objects is built in the ``GENERATE`` phase, the
``GENERATE`` options are used.  The profile data directory is the one of
the binary if the property is set on it, or else the one of the first
library holding such objects.  It is an error if CMake doesn't know how to
do it for their linker language.

The ``cmake_pgo_merge`` target name is reserved when profile guided
optimization is enabled for any target, whether or not the compiler needs
the profile data to be merged.  See :ref:`Target Names`.

See Also
^^^^^^^^

* The :prop_tgt:`INTERPROCEDURAL_OPTIMIZATION` target property, which
  is often combined with profile guided optimization.
