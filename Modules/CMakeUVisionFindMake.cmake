# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

find_package(UVision)
set(CMAKE_MAKE_PROGRAM ${UVISION_EXECUTABLE} CACHE FILEPATH "" FORCE)
mark_as_advanced(CMAKE_MAKE_PROGRAM)
