# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(Compiler/HP)
__compiler_hp(CXX)

# HP aCC since version 3.80 supports the flag +hpxstd98 to get ANSI C++98
# template support. It is known that version 6.25 doesn't need that flag.
# Current assumption: the flag is needed for every version from 3.80 to 4
# to get it working.
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 3.80)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4)
    set(CMAKE_CXX98_STANDARD_COMPILE_OPTION "+hpxstd98")
  else()
    set(CMAKE_CXX98_STANDARD_COMPILE_OPTION "")
  endif()
endif()

__compiler_check_default_language_standard(CXX 3.80 98)
