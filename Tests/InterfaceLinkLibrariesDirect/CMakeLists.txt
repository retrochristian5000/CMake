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

#----------------------------------------------------------------------------

# Test how original and injected dependencies get ordered.

# A bunch of static libraries that need to be linked in alphabetic order.
# Each library has an extra source to poison all symbols meant to be
# provided by earlier libraries.  This enforces ordering on platforms
# whose linkers re-visit static libraries.
add_library(orderA STATIC orderA.c)
add_library(orderB STATIC orderB.c orderB_poison.c)
add_library(orderC STATIC orderC.c orderC_poison.c)
add_library(orderD STATIC orderD.c orderD_poison.c)
add_library(orderE STATIC orderE.c orderE_poison.c)
add_library(orderF STATIC orderF.c orderF_poison.c)
add_library(orderG STATIC orderG.c orderG_poison.c)
add_library(orderH STATIC orderH.c orderH_poison.c)
add_library(orderI STATIC orderI.c orderI_poison.c)
add_library(orderJ STATIC orderJ.c orderJ_poison.c)

# An executable to drive linking.
add_executable(order_main order_main.c)

# In the following diagram, connection by a slash means the top
# target lists the bottom target in a link interface property:
#
#  \ => INTERFACE_LINK_LIBRARIES
#  / => INTERFACE_LINK_LIBRARIES_DIRECT
#
# The top of each tree represents an entry in the exe's LINK_LIBRARIES.
# CMake should evaluate this graph to generate the proper link order.
#
#             D        H
#            / \      / \
#           B   J    F   I
#          /   /    /   / \
#         A   C    E   G   J
set_property(TARGET order_main PROPERTY LINK_LIBRARIES orderD orderH)
set_property(TARGET orderD PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT orderB)
set_property(TARGET orderD PROPERTY INTERFACE_LINK_LIBRARIES orderJ)
set_property(TARGET orderB PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT orderA)
set_property(TARGET orderJ PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT orderC)
set_property(TARGET orderH PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT orderF)
set_property(TARGET orderH PROPERTY INTERFACE_LINK_LIBRARIES orderI)
set_property(TARGET orderF PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT orderE)
set_property(TARGET orderI PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT orderG)
set_property(TARGET orderI PROPERTY INTERFACE_LINK_LIBRARIES orderJ)

#----------------------------------------------------------------------------

# Test that the original LINK_LIBRARIES cannot be re-ordered by injection
# from usage requirements.

# A bunch of static libraries that need to be linked in alphabetic order.
# Each library has an extra source to poison all symbols meant to be
# provided by earlier libraries.  This enforces ordering on platforms
# whose linkers re-visit static libraries.
add_library(forceA STATIC orderA.c)
add_library(forceB STATIC orderB.c orderB_poison.c)
add_library(forceC STATIC orderC.c orderC_poison.c)
add_library(forceD STATIC orderD.c orderD_poison.c)
add_library(forceE STATIC orderE.c orderE_poison.c)
add_library(forceF STATIC orderF.c orderF_poison.c)
add_library(forceG STATIC orderG.c orderG_poison.c)
add_library(forceH STATIC orderH.c orderH_poison.c)
add_library(forceI STATIC orderI.c orderI_poison.c)
add_library(forceJ STATIC orderJ.c orderJ_poison.c)

# An executable to drive linking.
add_executable(force_main order_main.c)

# The executable explicitly lists all the libraries in the right order.
target_link_libraries(force_main PRIVATE forceA forceB forceC forceD forceE forceF forceG forceH)

# Add legitimate normal dependencies.
set_property(TARGET forceD PROPERTY INTERFACE_LINK_LIBRARIES forceJ)
set_property(TARGET forceH PROPERTY INTERFACE_LINK_LIBRARIES forceI)
set_property(TARGET forceI PROPERTY INTERFACE_LINK_LIBRARIES forceJ)

# Add bogus injected direct dependencies to verify that they do not
# change the original order of LINK_LIBRARIES.
set_property(TARGET forceA PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceB)
set_property(TARGET forceB PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceC)
set_property(TARGET forceC PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceD)
set_property(TARGET forceD PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceE)
set_property(TARGET forceE PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceF)
set_property(TARGET forceF PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceG)
set_property(TARGET forceG PROPERTY INTERFACE_LINK_LIBRARIES_DIRECT forceH)
