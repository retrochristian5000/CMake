function(check_pdb pdb created)
  get_filename_component(dir "${pdb}" DIRECTORY)
  get_filename_component(name "${pdb}" NAME)
  if(EXISTS "${RunCMake_TEST_BINARY_DIR}/${pdb}" OR
     EXISTS "${RunCMake_TEST_BINARY_DIR}/${dir}/Debug/${name}")
    if(NOT created)
      string(APPEND RunCMake_TEST_FAILED "Compiler PDB '${pdb}' created.\n")
    endif()
  elseif(created)
    string(APPEND RunCMake_TEST_FAILED "Compiler PDB '${pdb}' not created.\n")
  endif()
  set(RunCMake_TEST_FAILED "${RunCMake_TEST_FAILED}" PARENT_SCOPE)
endfunction()

foreach(pdb IN ITEMS
    reuse_old/creator.pdb
    reuse_default/creator.pdb
    reuse_default/reuse_default.pdb
    reuse_named/creator.pdb
    reuse_named/named.pdb
    reuse_legacy/creator.pdb
    reuse_legacy/reuse_legacy.pdb
    )
  check_pdb(${pdb} 1)
endforeach()

# With CMP0234 unset, the copy is the only compiler PDB.
foreach(pdb IN ITEMS reuse_old/reuse_old.pdb reuse_old/named_old.pdb)
  check_pdb(${pdb} 0)
endforeach()
