# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.


find_program(CMAKE_MAKE_PROGRAM
  NAMES ninja-build ninja samu
  NAMES_PER_DIR
  DOC "Program used to build from build.ninja files.")
mark_as_advanced(CMAKE_MAKE_PROGRAM)

if (CMAKE_MAKE_PROGRAM)
  # retrieve ninja version
  execute_process(COMMAND "${CMAKE_MAKE_PROGRAM}" --version
    RESULT_VARIABLE _CMAKE_NINJA_RESULT
    OUTPUT_VARIABLE CMAKE_MAKE_VERSION
    ERROR_VARIABLE CMAKE_MAKE_VERSION)
  if (NOT _CMAKE_NINJA_RESULT AND CMAKE_MAKE_VERSION MATCHES "[0-9]+(\\.[0-9]+)*")
    set (CMAKE_MAKE_VERSION "${CMAKE_MATCH_0}" CACHE STRING "Ninja version" FORCE)
    mark_as_advanced(CMAKE_MAKE_VERSION)
  endif()
endif()
