.. cmake-manual-description: CMake C++ Modules Support Reference

cmake-cxxmodules(7)
*******************

.. versionadded:: 3.28

C++ 20 introduced the concept of "modules" to the language.  The design
requires that build systems be able to order compilations among each other in
order to satisfy ``import`` statements reliably.  CMake's implementation uses
a scanning strategy where the dependencies of source files is discovered
during the build and these dependencies communicated to the build tool using
the appropriate facilities.

Scanning Control
================

Whether or not sources get scanned for C++ module usage is dependent on the
following queries. The first query that provides a yes/no answer is used.

- If the source file belongs to a file set of type ``CXX_MODULES``, it will
  be scanned.
- If the target does not use at least C++ 20, it will not be scanned.
- If the source file is not the language ``CXX``, it will not be scanned.
- If the :prop_sf:`CXX_SCAN_FOR_MODULES` source file property is set, its
  value will be used.
- If the :prop_tgt:`CXX_SCAN_FOR_MODULES` target property is set, its value
  will be used.  This target property is initialized by the
  :variable:`CMAKE_CXX_SCAN_FOR_MODULES` variable.
- Otherwise, it will be scanned (unless policy :policy:`CMP0155` is not set to
  ``NEW``).

Compiler Support
================

Compilers which CMake natively supports module dependency scanning include:

* MSVC 19.34 and newer (provided with Visual Studio 17.4 and newer)
* LLVM/Clang 16.0 and newer
* GCC 14 (for the in-development branch, after 2023-09-20) and newer

Generator Support
=================

The list of generators which support scanning sources for C++ modules include:

- :generator:`Ninja`
- :generator:`Ninja Multi-Config`
- :generator:`Visual Studio 17 2022`

Limitations
-----------

There are a number of known limitations of the current C++ module support in
CMake.  This does not document known limitations or bugs in compilers as these
can change over time.

For all generators:

- Header units are not supported.
- No builtin support for ``import std;`` or other compiler-provided modules.

For the Ninja Generators:

- ``ninja`` 1.11 or newer is required.

For the Visual Studio Generators:

- Only Visual Studio 2022 and toolchains newer than 19.34 (Visual Studio
  17.4).
- No support for exporting or installing BMI or module information.
- No diagnosis of using modules provided by ``PRIVATE`` sources from
  ``PUBLIC`` module sources.
