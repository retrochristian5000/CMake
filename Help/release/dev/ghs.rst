ghs
---

* The :generator:`Green Hills MULTI` generator is more complete:

  - Added support for architecture selection through
    :variable:`CMAKE_GENERATOR_PLATFORM`:
    ``ARM``, ``PPC``, and ``86``.

  - Added support for toolset selection through
    :variable:`CMAKE_GENERATOR_TOOLSET`,
    e.g. ``comp_201205``, ``comp_201510``, ``comp_201722_beta``.

  - Added support for platform selection through ``GHS_TARGET_PLATFORM``,
    e.g. ``integrity``, ``linux``, ``standalone``, etc.

  - Now honors user-set :variable:`CMAKE_C_COMPILER <CMAKE_<LANG>_COMPILER>`
    and :variable:`CMAKE_CXX_COMPILER <CMAKE_<LANG>_COMPILER>`.

  - No longer hard-codes ARM files, BSP, toolset, or OS locations.

  - Now allows user to specify ``GHS_OS_ROOT`` or default to ``C:/ghs``.

  - Now allows user to specify ``GHS_TOOLSET_ROOT``
    or default to ``C:/ghs``.

  - Compiler identification now recognizes GHS compilers.
