# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.


# Do NOT include this module directly into any of your code. It is meant as
# a library for Check*SourceRuns.cmake modules. It's content may change in
# any way between releases.

include_guard(GLOBAL)

macro(_CMAKE_CHECK_SOURCE_RUNS LANG LANG_TEXTUAL LANG_EXT SOURCE VAR)
  if(NOT DEFINED "${VAR}")
    set(MACRO_CHECK_FUNCTION_DEFINITIONS
      "-D${VAR} ${CMAKE_REQUIRED_FLAGS}")
    if(CMAKE_REQUIRED_LINK_OPTIONS)
      set(CHECK_${LANG}_SOURCE_COMPILES_ADD_LINK_OPTIONS
        LINK_OPTIONS ${CMAKE_REQUIRED_LINK_OPTIONS})
    else()
      set(CHECK_${LANG}_SOURCE_COMPILES_ADD_LINK_OPTIONS)
    endif()
    if(CMAKE_REQUIRED_LIBRARIES)
      set(CHECK_${LANG}_SOURCE_COMPILES_ADD_LIBRARIES
        LINK_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
    else()
      set(CHECK_${LANG}_SOURCE_COMPILES_ADD_LIBRARIES)
    endif()
    if(CMAKE_REQUIRED_INCLUDES)
      set(CHECK_${LANG}_SOURCE_COMPILES_ADD_INCLUDES
        "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}")
    else()
      set(CHECK_${LANG}_SOURCE_COMPILES_ADD_INCLUDES)
    endif()
    file(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.${LANG_EXT}"
      "${SOURCE}\n")

    if(NOT CMAKE_REQUIRED_QUIET)
      message(CHECK_START "Performing Test ${VAR}")
    endif()
    try_run(${VAR}_EXITCODE ${VAR}_COMPILED
      ${CMAKE_BINARY_DIR}
      ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.${LANG_EXT}
      COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
      ${CHECK_${LANG}_SOURCE_COMPILES_ADD_LINK_OPTIONS}
      ${CHECK_${LANG}_SOURCE_COMPILES_ADD_LIBRARIES}
      CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
      -DCMAKE_SKIP_RPATH:BOOL=${CMAKE_SKIP_RPATH}
      "${CHECK_${LANG}_SOURCE_COMPILES_ADD_INCLUDES}"
      COMPILE_OUTPUT_VARIABLE OUTPUT
      RUN_OUTPUT_VARIABLE RUN_OUTPUT)
    # if it did not compile make the return value fail code of 1
    if(NOT ${VAR}_COMPILED)
      set(${VAR}_EXITCODE 1)
    endif()
    # if the return value was 0 then it worked
    if("${${VAR}_EXITCODE}" EQUAL 0)
      set(${VAR} 1 CACHE INTERNAL "Test ${VAR}")
      if(NOT CMAKE_REQUIRED_QUIET)
        message(CHECK_PASS "Success")
      endif()
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Performing ${LANG_TEXTUAL} SOURCE FILE Test ${VAR} succeeded with the following compile output:\n"
        "${OUTPUT}\n"
        "...and run output:\n"
        "${RUN_OUTPUT}\n"
        "Return value: ${${VAR}}\n"
        "Source file was:\n${SOURCE}\n")
    else()
      if(CMAKE_CROSSCOMPILING AND "${${VAR}_EXITCODE}" MATCHES  "FAILED_TO_RUN")
        set(${VAR} "${${VAR}_EXITCODE}")
      else()
        set(${VAR} "" CACHE INTERNAL "Test ${VAR}")
      endif()

      if(NOT CMAKE_REQUIRED_QUIET)
        message(CHECK_FAIL "Failed")
      endif()
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Performing ${LANG_TEXTUAL} SOURCE FILE Test ${VAR} failed with the following compile output:\n"
        "${OUTPUT}\n"
        "...and run output:\n"
        "${RUN_OUTPUT}\n"
        "Return value: ${${VAR}_EXITCODE}\n"
        "Source file was:\n${SOURCE}\n")

    endif()
  endif()
endmacro()
