# The MSVC instrumented binaries need the PGO runtime library, which is not
# on the PATH without a developer environment.
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  get_filename_component(cl_dir "${CMAKE_C_COMPILER}" DIRECTORY)
  string(TOLOWER "${CMAKE_C_COMPILER_ARCHITECTURE_ID}" cl_arch)
  find_file(PGORT_DLL NAMES pgort140.dll
    HINTS "${cl_dir}" "${cl_dir}/../../${cl_arch}"
    NO_DEFAULT_PATH)
  if(PGORT_DLL)
    add_custom_command(TARGET main POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PGORT_DLL}" "$<TARGET_FILE_DIR:main>")
  endif()
endif()
