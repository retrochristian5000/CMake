# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(Compiler/HP)
__compiler_hp(Fortran)

set(CMAKE_Fortran_FORMAT_FIXED_FLAG "+source=fixed")
set(CMAKE_Fortran_FORMAT_FREE_FLAG "+source=free")
