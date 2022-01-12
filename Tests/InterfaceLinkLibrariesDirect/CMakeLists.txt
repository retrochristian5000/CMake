cmake_minimum_required(VERSION 3.21)
project(InterfaceLinkLibrariesDirect C)

include(testStaticLibPlugin.cmake)
add_executable(InterfaceLinkLibrariesDirect main.c)
target_link_libraries(InterfaceLinkLibrariesDirect PRIVATE testStaticLibWithPlugin)

include(testSharedLibWithHelper.cmake)
add_executable(UseSharedLibWithHelper UseSharedLibWithHelper.c)
target_link_libraries(UseSharedLibWithHelper PRIVATE testSharedLibWithHelper testSharedLibHelperExclude)

include(testExeWithPluginHelper.cmake)
add_library(ExePlugin MODULE ExePlugin.c)
target_link_libraries(ExePlugin PRIVATE testExeWithPluginHelper testExePluginHelperExclude)

#----------------------------------------------------------------------------

# Offer usage requirements and symbols to be used through static libs below.
add_library(A STATIC
  a_always.c

  # Good symbols that direct_from_A libraries poison if incorrectly used.
  a_not_direct_from_A.c
  a_not_direct_from_A_for_exe.c
  a_not_direct_from_A_optional.c

  # Bad symbols in direct_from_A libraries below to ensure they come first.
  a_poison_direct_from_A.c
  a_poison_direct_from_A_for_exe.c
  a_poison_direct_from_A_optional.c
  )

# Propagates as usage requirement from A.
add_library(direct_from_A STATIC direct_from_A.c direct_from_A_poison.c)
target_compile_definitions(direct_from_A INTERFACE DEF_DIRECT_FROM_A)
set_property(TARGET A APPEND PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT direct_from_A)

# Propagates as usage requirement from A, but only for executables.
add_library(direct_from_A_for_exe STATIC direct_from_A_for_exe.c direct_from_A_for_exe_poison.c)
target_compile_definitions(direct_from_A_for_exe INTERFACE DEF_DIRECT_FROM_A_FOR_EXE)
set_property(TARGET A APPEND PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT
  "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:direct_from_A_for_exe>")

# Propagates as usage requirement from A, but only for targets that opt-in.
add_library(direct_from_A_optional STATIC direct_from_A_optional.c direct_from_A_optional_poison.c)
target_compile_definitions(direct_from_A_optional INTERFACE DEF_DIRECT_FROM_A_OPTIONAL)
set_property(TARGET A APPEND PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT
  "$<$<BOOL:$<TARGET_PROPERTY:A_LINK_OPTIONAL>>:direct_from_A_optional>")

# Uses and propagates A's usage requirements.
# Does not use the exe-only or optional usage requirements.
add_library(static_A_public STATIC static_A_public.c)
target_link_libraries(static_A_public PUBLIC A)

# Uses A's usage requirements, but does not propagate them.
# Does not use the exe-only usage requirement.  Does use the optional one.
add_library(static_A_private STATIC static_A_private.c)
target_link_libraries(static_A_private PRIVATE A)
set_property(TARGET static_A_private PROPERTY A_LINK_OPTIONAL 1)

# Uses A's usage requirements, including an optional one.
add_executable(exe_use_static_A_public exe_use_static_A_public.c)
target_link_libraries(exe_use_static_A_public PRIVATE static_A_public)
set_property(TARGET exe_use_static_A_public PROPERTY A_LINK_OPTIONAL 1)

# Does not use A's usage requirements.
add_executable(exe_use_static_A_private exe_use_static_A_private.c)
target_link_libraries(exe_use_static_A_private PRIVATE static_A_private)
