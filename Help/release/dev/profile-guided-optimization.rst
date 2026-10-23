profile-guided-optimization
---------------------------

* The :prop_tgt:`PROFILE_GUIDED_OPTIMIZATION` target property and the
  associated :variable:`CMAKE_PROFILE_GUIDED_OPTIMIZATION` variable were
  added to build targets with profile guided optimization (PGO) using
  the GNU, Clang, AppleClang, and MSVC compilers.

* The :prop_tgt:`PGO_DATA_DIRECTORY` target property and the associated
  :variable:`CMAKE_PGO_DATA_DIRECTORY` variable were added to specify
  where profile data is stored.

* With Clang and MSVC, a ``cmake_pgo_merge`` target is generated to merge
  the profile data, using the new
  :variable:`CMAKE_<LANG>_COMPILER_LLVM_PROFDATA` tool with Clang.
