enable_language(Fortran)

# See #27894
if (NOT CMAKE_SIZEOF_VOID_P)
  message(FATAL_ERROR
    "`CMAKE_SIZEOF_VOID_P` not initialized from toolchain data")
endif ()
