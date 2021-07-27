include(RunCMake)

run_cmake(NoLangSHARED)
run_cmake(LINKER_LANGUAGE-genex)
run_cmake(link-libraries-TARGET_FILE-genex)
run_cmake(link-libraries-TARGET_FILE-genex-ok)

run_cmake(DetermineFail)

run_cmake(ExternalCUDA)

run_cmake(CMP0127-OLD)
if(";${CMAKE_C_COMPILER_ID};${CMAKE_C_SIMULATE_ID};" MATCHES ";MSVC;")
  set(RunCMake-stderr-file "CMP0127-WARN-stderr-msvc.txt")
endif()
run_cmake(CMP0127-WARN)
unset(RunCMake-stderr-file)
