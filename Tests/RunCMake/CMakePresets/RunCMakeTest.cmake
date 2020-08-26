include(RunCMake)

function(run_cmake_presets name)
  set(RunCMake_TEST_SOURCE_DIR "${RunCMake_BINARY_DIR}/${name}")
  set(_source_arg "${RunCMake_TEST_SOURCE_DIR}")
  if(CMakePresets_RELATIVE_SOURCE)
    set(_source_arg "../${name}")
  endif()
  file(REMOVE_RECURSE "${RunCMake_TEST_SOURCE_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_SOURCE_DIR}")
  configure_file("${RunCMake_SOURCE_DIR}/CMakeLists.txt.in" "${RunCMake_TEST_SOURCE_DIR}/CMakeLists.txt" @ONLY)

  if(NOT CMakePresets_FILE)
    set(CMakePresets_FILE "${RunCMake_SOURCE_DIR}/${name}.json.in")
  endif()
  if(EXISTS "${CMakePresets_FILE}")
    configure_file("${CMakePresets_FILE}" "${RunCMake_TEST_SOURCE_DIR}/CMakePresets.json" @ONLY)
  endif()

  if(NOT CMakeUserPresets_FILE)
    set(CMakeUserPresets_FILE "${RunCMake_SOURCE_DIR}/${name}User.json.in")
  endif()
  if(EXISTS "${CMakeUserPresets_FILE}")
    configure_file("${CMakeUserPresets_FILE}" "${RunCMake_TEST_SOURCE_DIR}/CMakeUserPresets.json" @ONLY)
  endif()

  set(_s_arg -S)
  if(CMakePresets_NO_S_ARG)
    set(_s_arg)
  endif()

  set(RunCMake_TEST_COMMAND ${CMAKE_COMMAND}
    ${_s_arg} ${_source_arg}
    -DRunCMake_TEST=${name}
    -DRunCMake_GENERATOR=${RunCMake_GENERATOR}
    -DCMAKE_MAKE_PROGRAM=${RunCMake_MAKE_PROGRAM}
    --no-warn-unused-cli
    --preset=${name}
    ${ARGN}
    )
  run_cmake(${name})
endfunction()

# Test CMakePresets.json errors
run_cmake_presets(NoCMakePresets)
run_cmake_presets(JSONParseError)
run_cmake_presets(InvalidRoot)
run_cmake_presets(NoVersion)
run_cmake_presets(InvalidVersion)
run_cmake_presets(LowMajorVersion)
run_cmake_presets(HighMajorVersion)
run_cmake_presets(LowMinorVersion)
run_cmake_presets(HighMinorVersion)
run_cmake_presets(NoPresets)
run_cmake_presets(InvalidPresets)
run_cmake_presets(PresetNotObject)
run_cmake_presets(NoPresetName)
run_cmake_presets(InvalidPresetName)
run_cmake_presets(EmptyPresetName)
run_cmake_presets(NoPresetGenerator)
run_cmake_presets(InvalidPresetGenerator)
run_cmake_presets(NoPresetBinaryDir)
run_cmake_presets(InvalidPresetBinaryDir)
run_cmake_presets(InvalidVariables)
run_cmake_presets(VariableNotObject)
run_cmake_presets(NoVariableValue)
run_cmake_presets(InvalidVariableValue)
run_cmake_presets(ExtraRootField)
run_cmake_presets(ExtraPresetField)
run_cmake_presets(ExtraVariableField)
run_cmake_presets(DuplicatePresets)
run_cmake_presets(CyclicInheritance0)
run_cmake_presets(CyclicInheritance1)
run_cmake_presets(CyclicInheritance2)
run_cmake_presets(InvalidInheritance)

# Test cmakeMinimumRequired field
run_cmake_presets(MinimumRequiredInvalid)
run_cmake_presets(MinimumRequiredEmpty)
run_cmake_presets(MinimumRequiredMajor)
run_cmake_presets(MinimumRequiredMinor)
run_cmake_presets(MinimumRequiredPatch)

# Test properly working CMakePresets.json
set(CMakePresets_FILE "${RunCMake_SOURCE_DIR}/CMakePresets.json.in")
set(ENV{TEST_ENVIRONMENT_VARIABLE} "Environment variable is working")
unset(ENV{TEST_NO_ENVIRONMENT_VARIABLE})
run_cmake_presets(Good "-DTEST_OVERRIDE_1=Overridden value" "-DTEST_OVERRIDE_2:STRING=Overridden value" -C "${RunCMake_SOURCE_DIR}/CacheOverride.cmake" "-UTEST_UNDEF")
unset(ENV{TEST_ENVIRONMENT_VARIABLE})
run_cmake_presets(GoodNoArgs)
file(REMOVE_RECURSE ${RunCMake_BINARY_DIR}/GoodBinaryUp-build)
run_cmake_presets(GoodBinaryUp)
set(CMakePresets_RELATIVE_SOURCE TRUE)
run_cmake_presets(GoodBinaryRelative)
unset(CMakePresets_RELATIVE_SOURCE)
run_cmake_presets(GoodSpaces "--preset=Good Spaces")
if(WIN32)
  run_cmake_presets(GoodWindowsBackslash)
endif()
set(CMakePresets_FILE "${RunCMake_SOURCE_DIR}/GoodBOM.json.in")
run_cmake_presets(GoodBOM)
set(CMakePresets_FILE "${RunCMake_SOURCE_DIR}/CMakePresets.json.in")
file(REMOVE_RECURSE ${RunCMake_BINARY_DIR}/GoodBinaryCmdLine-build)
run_cmake_presets(GoodBinaryCmdLine -B ${RunCMake_BINARY_DIR}/GoodBinaryCmdLine-build)
run_cmake_presets(GoodGeneratorCmdLine -G ${RunCMake_GENERATOR})
run_cmake_presets(InvalidGeneratorCmdLine -G "Invalid Generator")
set(CMakePresets_NO_S_ARG TRUE)
run_cmake_presets(GoodNoS)
unset(CMakePresets_NO_S_ARG)
run_cmake_presets(GoodInheritanceParent)
run_cmake_presets(GoodInheritanceChild)
run_cmake_presets(GoodInheritanceOverride)
run_cmake_presets(GoodInheritanceMulti)
run_cmake_presets(GoodInheritanceMultiSecond)
run_cmake_presets(GoodInheritanceMacro)

# Test bad preset arguments
run_cmake_presets(UnclosedMacro)
run_cmake_presets(NoSuchMacro)
run_cmake_presets(InvalidGenerator)

# Test Visual Studio-specific stuff
if(RunCMake_GENERATOR MATCHES "^Visual Studio ")
  run_cmake_presets(VisualStudioGeneratorArch)
  run_cmake_presets(VisualStudioWin32)
  run_cmake_presets(VisualStudioWin64)
  run_cmake_presets(VisualStudioWin32Override -A x64)
  run_cmake_presets(VisualStudioToolset)
  run_cmake_presets(VisualStudioToolsetOverride -T "Test Toolset")
  run_cmake_presets(VisualStudioInheritanceParent)
  run_cmake_presets(VisualStudioInheritanceChild)
  run_cmake_presets(VisualStudioInheritanceOverride)
  run_cmake_presets(VisualStudioInheritanceMulti)
  run_cmake_presets(VisualStudioInheritanceMultiSecond)
endif()

# Test bad command line arguments
run_cmake_presets(NoSuchPreset)
run_cmake_presets(NoPresetArgument --preset=)
run_cmake_presets(UseHiddenPreset)

# Test CMakeUserPresets.json
unset(CMakePresets_FILE)
run_cmake_presets(GoodUserOnly)
run_cmake_presets(GoodUserFromMain)
run_cmake_presets(GoodUserFromUser)

# Test CMakeUserPresets.json errors
run_cmake_presets(UserDuplicateInUser)
run_cmake_presets(UserDuplicateCross)
run_cmake_presets(UserInheritance)

# Test listing presets
set(CMakePresets_FILE "${RunCMake_SOURCE_DIR}/ListPresets.json.in")
run_cmake_presets(ListPresets --list-presets)
run_cmake_presets(ListPresetsNoSuchPreset)
run_cmake_presets(ListPresetsHidden)
