# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(Compiler/SunPro)

__compiler_sunpro(Fortran)

set(CMAKE_Fortran_FORMAT_FIXED_FLAG "-fixed")
set(CMAKE_Fortran_FORMAT_FREE_FLAG "-free")

set(CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG "-R")

set(CMAKE_Fortran_MODDIR_FLAG "-moddir=")
set(CMAKE_Fortran_MODPATH_FLAG "-M")

set(CMAKE_Fortran_PREPROCESS_SOURCE
  "<CMAKE_Fortran_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -F -fpp <SOURCE> -o <PREPROCESSED_SOURCE>")

set(CMAKE_Fortran_CREATE_PREPROCESSED_SOURCE "<CMAKE_Fortran_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -F -fpp <SOURCE> -o <PREPROCESSED_SOURCE>")
