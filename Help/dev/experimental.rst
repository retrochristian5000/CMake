CMake Experimental Features Guide
*********************************

The following is a guide to CMake experimental features that are
under development and not yet included in official documentation.
See documentation on `CMake Development`_ for more information.

.. _`CMake Development`: README.rst

C++20 Module Dependencies
=========================

The Ninja generator has experimental infrastructure supporting C++20 module
dependency scanning.  This is similar to the Fortran modules support, but
relies on external tools to scan C++20 translation units for module
dependencies.  The approach is described by Kitware's `D1483r1`_ paper.

The ``CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP`` variable can be set to ``1``
in order to activate this undocumented experimental infrastructure.  This
is **intended to make the functionality available to compiler writers** so
they can use it to develop and test their dependency scanning tool.
The ``CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE`` variable must also be set
to tell CMake how to invoke the C++20 module dependency scanning tool.

For example, add code like the following to a test project:

.. code-block:: cmake

  set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
  string(CONCAT CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE
    "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE>"
    " -MT <DYNDEP_FILE> -MD -MF <DEP_FILE>"
    " ${flags_to_scan_deps} -fdep-file=<DYNDEP_FILE> -fdep-output=<OBJECT>"
    )

The tool specified by ``CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE`` is
expected to process the translation unit, write preprocessor dependencies
to the file specified by the ``<DEP_FILE>`` placeholder, and write module
dependencies to the file specified by the ``<DYNDEP_FILE>`` placeholder.

The module dependencies should be written in the format described
by the `P1689r2`_ paper, plus some revisions to be made in P1689r3
and already implemented by this version of CMake:

* The ``work-directory`` field is specified by P1689r2 as a top-level
  member.  P1689r3 moves inside each entry in the ``rules`` array.

* P1689r2 specifies a way to represent arbitrary binary file names.
  P1689r3 removes that for simplicity, and expects all paths to be
  representable as UTF-8.

FIXME: link to P1689r3 when published.

Compiler writers may try out their scanning functionality using
the `cxx-modules-sandbox`_ test project, modified to set variables
as above for their compiler.

For compilers that generate module maps, tell CMake as follows:

.. code-block:: cmake

  set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FORMAT "gcc")
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FLAG
    "${compiler_flags_for_module_map} -fmodule-mapper=<MODULE_MAP_FILE>")

.. _`D1483r1`: https://mathstuf.fedorapeople.org/fortran-modules/fortran-modules.html
.. _`P1689r2`: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1689r2.html
.. _`cxx-modules-sandbox`: https://github.com/mathstuf/cxx-modules-sandbox
