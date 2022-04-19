include(RunCMake)

# Test negative cases where C++20 modules do not work.
run_cmake(NoCXX)
run_cmake(NoCXX20)
run_cmake(NoCXX20ModuleFlag)

# Test behavior when the generator does not support C++20 modules.
if (NOT RunCMake_GENERATOR MATCHES "Ninja")
  run_cmake(NoDyndepSupport)

  # Bail; the remaining tests require the generator to successfully generate
  # with C++20 modules in the source list.
  return ()
endif ()

set(fileset_types
  Modules
  ModuleHeaderUnits
  ModuleInternalPartitions)
set(scopes
  Interface
  Private
  Public)
foreach (fileset_type IN LISTS fileset_types)
  foreach (scope IN LISTS scopes)
    run_cmake("FileSet${fileset_type}${scope}")
  endforeach ()

  # Test the error message when a non-C++ source file is found in the source
  # list.
  run_cmake("NotCXXSource${fileset_type}")
endforeach ()

run_cmake(InstallBMI)
run_cmake(InstallBMIGenericArgs)
run_cmake(InstallBMIIgnore)
run_cmake(ExportBuildCxxModules)
run_cmake(ExportInstallCxxModules)

# Generator-specific tests.
if (RunCMake_GENERATOR MATCHES "Ninja")
  run_cmake(NinjaDependInfoFileSet)
  run_cmake(NinjaDependInfoExport)
else ()
  message(FATAL_ERROR
    "Please add `DependInfo` tests for the `${RunCMake_GENERATOR}` generator.")
endif ()
