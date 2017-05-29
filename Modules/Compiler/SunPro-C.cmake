# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(Compiler/SunPro)

__compiler_sunpro(C)

set(CMAKE_C_CREATE_PREPROCESSED_SOURCE "<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -E <SOURCE> > <PREPROCESSED_SOURCE>")

# Initialize C link type selection flags.  These flags are used when
# building a shared library, shared module, or executable that links
# to other libraries to select whether to use the static or shared
# versions of the libraries.
foreach(type SHARED_LIBRARY SHARED_MODULE EXE)
  set(CMAKE_${type}_LINK_STATIC_C_FLAGS "-Bstatic")
  set(CMAKE_${type}_LINK_DYNAMIC_C_FLAGS "-Bdynamic")
endforeach()

if (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 5.13)
  set(CMAKE_C90_STANDARD_COMPILE_OPTION "-std=c89")
  set(CMAKE_C90_EXTENSION_COMPILE_OPTION "-std=c89")
  set(CMAKE_C99_STANDARD_COMPILE_OPTION "-std=c99")
  set(CMAKE_C99_EXTENSION_COMPILE_OPTION "-std=c99")
  set(CMAKE_C11_STANDARD_COMPILE_OPTION "-std=c11")
  set(CMAKE_C11_EXTENSION_COMPILE_OPTION "-std=c11")
elseif (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 5.11)
  set(CMAKE_C90_STANDARD_COMPILE_OPTION "")
  set(CMAKE_C90_EXTENSION_COMPILE_OPTION "")
  set(CMAKE_C99_STANDARD_COMPILE_OPTION "-xc99")
  set(CMAKE_C99_EXTENSION_COMPILE_OPTION "-xc99")
endif()

__compiler_check_default_language_standard(C 5.11 90 5.14 11)
