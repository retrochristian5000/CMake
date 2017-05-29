# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# This module is shared by multiple languages; use include blocker.
if(__COMPILER_SUNPRO)
  return()
endif()
set(__COMPILER_SUNPRO 1)

include(Compiler/CMakeCommonCompilerMacros)

macro(__compiler_sunpro)
  set(CMAKE_${lang}_VERBOSE_FLAG "-#")

  set(CMAKE_${lang}_COMPILE_OPTIONS_PIC -KPIC)
  set(CMAKE_SHARED_LIBRARY_${lang}_FLAGS "-KPIC")
  set(CMAKE_SHARED_LIBRARY_CREATE_${lang}_FLAGS "-G")
  set(CMAKE_SHARED_LIBRARY_RUNTIME_${lang}_FLAG "-R")
  set(CMAKE_SHARED_LIBRARY_RUNTIME_${lang}_FLAG_SEP ":")
  set(CMAKE_SHARED_LIBRARY_SONAME_${lang}_FLAG "-h")

  string(APPEND CMAKE_${lang}_FLAGS_INIT " ")
  string(APPEND CMAKE_${lang}_FLAGS_DEBUG_INIT " -g")
  string(APPEND CMAKE_${lang}_FLAGS_MINSIZEREL_INIT " -xO2 -xspace -DNDEBUG")
  string(APPEND CMAKE_${lang}_FLAGS_RELEASE_INIT " -xO3 -DNDEBUG")
  string(APPEND CMAKE_${lang}_FLAGS_RELWITHDEBINFO_INIT " -g -xO2 -DNDEBUG")

  set(CMAKE_${lang}_CREATE_ASSEMBLY_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -S <SOURCE> -o <ASSEMBLY_SOURCE>")
endmacro()
