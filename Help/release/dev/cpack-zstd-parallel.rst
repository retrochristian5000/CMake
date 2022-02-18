cpack-zstd-parallel
------------------

* CPack can now compress `zstd`-compressed packages in parallel
  by setting `CPACK_THREADS` when compiled with libarchive <= 3.6,
  which is the case for the official CMake binaries.
