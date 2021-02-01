intel-llvm-compilers
--------------------

* The Intel oneAPI NextGen LLVM compilers are now supported with
  compiler id ``IntelLLVM``:

  * CMake supports the ``icx``/``icpx`` C/C++ compilers on Linux, and
    the ``icx`` C/C++ compiler on Windows, in oneAPI 2021.1 and later.

  * CMake is prepared to support the ``icx`` Fortran compiler with
    the ``IntelLLVM`` compiler id on Linux and Windows in a future
    oneAPI release.  In oneAPI 2021.1, the ``ifx`` compiler is available
    only on Linux and does not define the necessary identification macros.
    CMake identifies it as the classic ``Intel`` compiler, which works
    in many cases because it accepts the same command line parameters
    as ``ifort``.

  The Intel oneAPI Classic compilers (``icc``, ``icpc``, and ``ifort``)
  continue to be supported with compiler id ``Intel``.
