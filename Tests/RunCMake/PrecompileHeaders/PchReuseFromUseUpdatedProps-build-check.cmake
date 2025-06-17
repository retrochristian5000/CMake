if(MSVC OR (CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_SIMULATE_ID STREQUAL "MSVC"))
  find_file(pdb_file
    NAMES custom-post.pdb custom-post-debug.pdb
    PATHS "${RunCMake_TEST_BINARY_DIR}"
    PATH_SUFFIXES NOT_USED NOT_USED_DEBUG)
  if (NOT EXISTS "${pdb_file}")
    list(APPEND RunCMake_TEST_FAILED
      "PDB file was not created from properties updated after reuse link")
  endif ()

  if (NOT EXISTS "${RunCMake_TEST_BINARY_DIR}/custom" AND
      NOT EXISTS "${RunCMake_TEST_BINARY_DIR}/custom-debug")
    list(APPEND RunCMake_TEST_FAILED
      "Updated PDB output directory ('custom' or 'custom-debug') was not created")
  endif ()
endif ()
