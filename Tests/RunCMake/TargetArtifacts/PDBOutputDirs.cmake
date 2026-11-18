cmake_policy(SET CMP0141 NEW)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
enable_language(C)

# Exercise the owning target context for both properties, with and without
# a configuration-specific override.  Distinct target names must produce
# distinct PDB directories.
foreach(kind IN ITEMS default config)
  add_executable(exe_${kind} main.c)
  add_library(shared_${kind} SHARED lib.c)
  foreach(type IN ITEMS exe shared)
    set(tgt ${type}_${kind})
    set_target_properties(${tgt} PROPERTIES
      PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pdb/$<TARGET_PROPERTY:NAME>"
      COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cpdb/$<TARGET_PROPERTY:NAME>"
      PDB_NAME "${tgt}-link"
      COMPILE_PDB_NAME "${tgt}-compile"
      )
    if(kind STREQUAL "config")
      set_target_properties(${tgt} PROPERTIES
        PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/pdb-debug/$<TARGET_PROPERTY:NAME>"
        COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/cpdb-debug/$<TARGET_PROPERTY:NAME>"
        )
    endif()
    if(MSVC)
      # The path check is independent of TARGET_PDB_FILE's path computation.
      # Debug builds must emit real compiler and linker PDBs at these paths.
      add_custom_command(TARGET ${tgt} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
          "-Dartifact_path=${CMAKE_CURRENT_BINARY_DIR}"
          "-Dtarget_name=${tgt}"
          "-Ddirectory_kind=${kind}"
          -P "${CMAKE_CURRENT_SOURCE_DIR}/checkPDBOutputDirs.cmake"
        VERBATIM
        )
    endif()
  endforeach()
endforeach()
