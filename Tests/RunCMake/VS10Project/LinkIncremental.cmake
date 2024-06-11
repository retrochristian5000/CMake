set(CMAKE_CONFIGURATION_TYPES Debug Release MinSizeRel RelWithDebInfo)
cmake_policy(SET CMP0171 NEW)
enable_language(C)
enable_language(CXX)

add_library(cmake-default-C SHARED empty.c)
add_library(cmake-default-CXX SHARED empty.cxx)

set(CMAKE_MSVC_INCREMENTAL_LINK "")
add_library(empty-C SHARED empty.c)
add_library(empty-CXX SHARED empty.cxx)

set(CMAKE_MSVC_INCREMENTAL_LINK "Force")
add_library(Force-C SHARED empty.c)
add_library(Force-CXX SHARED empty.cxx)

set(CMAKE_MSVC_INCREMENTAL_LINK "Default")
add_library(Default-C SHARED empty.c)
add_library(Default-CXX SHARED empty.cxx)

add_library(Never-C SHARED empty.c)
set_property(TARGET Never-C PROPERTY MSVC_INCREMENTAL_LINK "Never")
add_library(Never-CXX SHARED empty.cxx)
set_property(TARGET Never-CXX PROPERTY MSVC_INCREMENTAL_LINK "Never")
