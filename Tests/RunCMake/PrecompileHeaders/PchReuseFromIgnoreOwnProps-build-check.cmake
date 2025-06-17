if(MSVC OR (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_SIMULATE_ID STREQUAL "MSVC"))
  find_file(pdb_file
    NAMES NOT_USED.pdb
    PATHS "${RunCMake_TEST_BINARY_DIR}"
    PATH_SUFFIXES NOT_USED NOT_USED_DEBUG)
  if (EXISTS "${pdb_file}")
    list(APPEND RunCMake_TEST_FAILED
      "PDB file was created from properties meant to be ignored")
  endif ()

  if (EXISTS "${RunCMake_TEST_BINARY_DIR}/NOT_USED")
    list(APPEND RunCMake_TEST_FAILED
      "PDB output directory ('NOT_USED') was created")
  endif ()
  if (EXISTS "${RunCMake_TEST_BINARY_DIR}/NOT_USED_DEBUG")
    list(APPEND RunCMake_TEST_FAILED
      "PDB output directory ('NOT_USED_DEBUG') was created")
  endif ()
endif ()
