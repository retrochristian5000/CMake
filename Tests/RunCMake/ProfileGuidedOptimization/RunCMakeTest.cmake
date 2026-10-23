include(RunCMake)

run_cmake(Inspect)
include("${RunCMake_BINARY_DIR}/Inspect-build/info.cmake")

run_cmake(InvalidValue)
run_cmake(InvalidValueNoLanguage)

function(run_cmake_config case config)
  if(RunCMake_GENERATOR_IS_MULTI_CONFIG)
    set(RunCMake_TEST_OPTIONS -DCMAKE_CONFIGURATION_TYPES=${config})
  else()
    set(RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=${config})
  endif()
  run_cmake(${case})
endfunction()
run_cmake_config(InvalidValueGenex Release)

run_cmake(NotSupported)
run_cmake(NotSupportedReason)
run_cmake(Disabled)

if(NOT CMAKE_C_COMPILE_OPTIONS_PGO_GENERATE
    OR RunCMake_GENERATOR MATCHES "Watcom|Borland")
  return()
endif()

run_cmake(ReservedTargetName)
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  run_cmake(DirectoryConflict)
endif()
if(CMAKE_C_PGO_MERGE)
  run_cmake_config(SubdirectoryLanguage Release)
endif()

function(run_LinkOnly case)
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/${case}-build")
  if(RunCMake_GENERATOR_IS_MULTI_CONFIG)
    set(RunCMake_TEST_OPTIONS -DCMAKE_CONFIGURATION_TYPES=Release)
    set(exe_dir "${RunCMake_TEST_BINARY_DIR}/Release")
  else()
    set(RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=Release)
    set(exe_dir "${RunCMake_TEST_BINARY_DIR}")
  endif()
  run_cmake(${case})
  set(RunCMake_TEST_NO_CLEAN 1)
  run_cmake_command(${case}-build
    ${CMAKE_COMMAND} --build . --config Release)
  run_cmake_command(${case}-run "${exe_dir}/main")
endfunction()
run_LinkOnly(LibraryOnly)
run_LinkOnly(LibraryDirectory)
run_LinkOnly(ObjectLibrarySources)
run_LinkOnly(StaticLibraryObjectLibrary)
# Clang fails to compile in the USE phase without profile data.
if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  run_LinkOnly(LibraryGenerateMainUse)
endif()

# Clang needs 'llvm-profdata' to merge the profile data.
if(CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_C_PGO_MERGE)
  return()
endif()

function(run_Workflow)
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/Workflow-build")
  set(RunCMake_TEST_NO_CLEAN 1)
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")
  if(RunCMake_GENERATOR_IS_MULTI_CONFIG)
    set(RunCMake_TEST_OPTIONS -DCMAKE_CONFIGURATION_TYPES=Release)
    set(pgo_dir "${RunCMake_TEST_BINARY_DIR}/pgo/Release")
    set(exe_dir "${RunCMake_TEST_BINARY_DIR}/Release")
  else()
    set(RunCMake_TEST_OPTIONS -DCMAKE_BUILD_TYPE=Release)
    set(pgo_dir "${RunCMake_TEST_BINARY_DIR}/pgo")
    set(exe_dir "${RunCMake_TEST_BINARY_DIR}")
  endif()

  list(APPEND RunCMake_TEST_OPTIONS -DCMAKE_PROFILE_GUIDED_OPTIMIZATION=GENERATE)
  run_cmake(Workflow)
  # Show the linker output, e.g. of 'cmake -E vs_link_exe'.
  run_cmake_command(Workflow-generate-build
    ${CMAKE_COMMAND} -E env VERBOSE=1
    ${CMAKE_COMMAND} --build . --config Release)
  if(EXISTS "${RunCMake_TEST_BINARY_DIR}/pgo-not-available")
    message(STATUS "Workflow - SKIPPED: profile guided optimization is not "
      "available in this edition of the MSVC linker")
    return()
  endif()
  if(CMAKE_C_PGO_MERGE)
    block()
      set(RunCMake_TEST_OUTPUT_MERGE 1)
      run_cmake_command(Workflow-merge-no-data
        ${CMAKE_COMMAND} --build . --config Release --target cmake_pgo_merge)
    endblock()
  endif()
  run_cmake_command(Workflow-train "${exe_dir}/main")
  if(CMAKE_C_PGO_MERGE)
    run_cmake_command(Workflow-merge
      ${CMAKE_COMMAND} --build . --config Release --target cmake_pgo_merge)
    # Merging again without new raw profile data keeps the merged data.
    if(CMAKE_C_PGO_MERGE_OUTPUT)
      block()
        set(RunCMake_TEST_OUTPUT_MERGE 1)
        run_cmake_command(Workflow-merge-again
          ${CMAKE_COMMAND} --build . --config Release --target cmake_pgo_merge)
      endblock()
    endif()
    # Merging the profile data of another training run accumulates it.
    run_cmake_command(Workflow-train-again "${exe_dir}/main")
    run_cmake_command(Workflow-merge-accumulate
      ${CMAKE_COMMAND} --build . --config Release --target cmake_pgo_merge)
  endif()

  run_cmake_command(Workflow-use-configure
    ${CMAKE_COMMAND} -DCMAKE_PROFILE_GUIDED_OPTIMIZATION=USE .)
  # Show the linker output, e.g. of 'cmake -E vs_link_exe'.
  run_cmake_command(Workflow-use-build
    ${CMAKE_COMMAND} -E env VERBOSE=1
    ${CMAKE_COMMAND} --build . --config Release)

  # Profile data does not depend on the location of the build tree.
  if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR
      CMAKE_C_COMPILE_OPTIONS_PGO_GENERATE MATCHES "-fprofile-prefix-path")
    set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/WorkflowOtherTree-build")
    file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
    list(FILTER RunCMake_TEST_OPTIONS EXCLUDE REGEX "PROFILE_GUIDED")
    list(APPEND RunCMake_TEST_OPTIONS
      -DCMAKE_PROFILE_GUIDED_OPTIMIZATION=USE
      "-DCMAKE_PGO_DATA_DIRECTORY=${pgo_dir}"
      )
    run_cmake(WorkflowOtherTree)
    run_cmake_command(WorkflowOtherTree-build
      ${CMAKE_COMMAND} --build . --config Release)
  endif()
endfunction()
run_Workflow()
