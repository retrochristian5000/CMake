# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(Compiler/SunPro)

__compiler_sunpro(CXX)

set(CMAKE_CXX_CREATE_PREPROCESSED_SOURCE "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -E <SOURCE> > <PREPROCESSED_SOURCE>")

# Create archives with "CC -xar" in case user adds "-instances=extern"
# so that template instantiations are available to archive members.
set(CMAKE_CXX_CREATE_STATIC_LIBRARY
  "<CMAKE_CXX_COMPILER> -xar -o <TARGET> <OBJECTS> "
  "<CMAKE_RANLIB> <TARGET> ")

# Initialize C link type selection flags.  These flags are used when
# building a shared library, shared module, or executable that links
# to other libraries to select whether to use the static or shared
# versions of the libraries.
foreach(type SHARED_LIBRARY SHARED_MODULE EXE)
  set(CMAKE_${type}_LINK_STATIC_CXX_FLAGS "-Bstatic")
  set(CMAKE_${type}_LINK_DYNAMIC_CXX_FLAGS "-Bdynamic")
endforeach()

if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5.13)
  set(CMAKE_CXX98_STANDARD_COMPILE_OPTION "-std=c++03")
  set(CMAKE_CXX98_EXTENSION_COMPILE_OPTION "-std=c++03")
  set(CMAKE_CXX11_STANDARD_COMPILE_OPTION "-std=c++11")
  set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-std=c++11")
  set(CMAKE_CXX_LINK_WITH_STANDARD_COMPILE_OPTION 1)
else()
  set(CMAKE_CXX98_STANDARD_COMPILE_OPTION "-library=stlport4")
  set(CMAKE_CXX98_EXTENSION_COMPILE_OPTION "-library=stlport4")
  set(CMAKE_CXX_LINK_WITH_STANDARD_COMPILE_OPTION 1)
endif()

__compiler_check_default_language_standard(CXX 0 98)
