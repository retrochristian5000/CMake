# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=[

NOTE: This function is used internally by CMake. Projects should not include
      this file directly.

The cmake_check_compiler_flag() function can be used to compile and link a
source file to check whether a specific compiler or linker flag is supported.
The function does not use the try_compile() command so as to avoid infinite
recursion.  It may not work for all platforms or toolchains, the caller is
responsible for ensuring it is only called in valid situations.

Parameters:
  lang   - Language to check.
  flag   - The flag to add to the compile/link command line.
  result - Boolean output variable.  It will be stored in the cache as an
           internal variable and if true, will cause future tests that assign
           to that variable to be bypassed.

Optional parameters:
  SRC_EXT         - Overrides the extension of the source file used for the
                    check.  Defaults are 'c' (C), 'cxx' (CXX), 'F' (Fortran).
  COMMAND_PATTERN - Pattern to be used for the command line. The default is
                    '<FLAG> -o <OUTPUT> <SOURCE>'
  FAIL_REGEX      - List of additional regular expressions that, if matched by
                    the output, give a failed result for the check.  A common
                    set of regular expressions will be included in addition to
                    those given by FAIL_REGEX.

#]=]

include_guard(GLOBAL)
include(CMakeCheckCompilerFlagCommonPatterns)

function(CMAKE_CHECK_COMPILER_FLAG lang flag result)
  # Cache results between runs similar to check_<lang>_source_compiles()
  if(DEFINED ${result})
    return()
  endif()

  if (NOT lang MATCHES "^(C|CXX|Fortran)$")
    message(FATAL_ERROR
      "CMAKE_CHECK_COMPILER_FLAG: ${lang}: Unsupported language.")
  endif()

  cmake_parse_arguments(CCCF "" "SRC_EXT;COMMAND_PATTERN" "FAIL_REGEX" ${ARGN})

  if (NOT CCCF_COMMAND_PATTERN)
    set (CCCF_COMMAND_PATTERN "<FLAG> -o <OUTPUT> <SOURCE>")
  endif()

  list (APPEND CCCF_FAIL_REGEX "argument unused during compilation") # clang
  if (lang STREQUAL "C")
    list(APPEND CCCF_FAIL_REGEX
      "command line option .* is valid for .* but not for C") # GNU
  elseif(lang STREQUAL "CXX")
    list(APPEND CCCF_FAIL_REGEX
      "command line option .* is valid for .* but not for C\\+\\+") # GNU
  elseif(lang STREQUAL "Fortran")
    list(APPEND CCCF_FAIL_REGEX
      "command line option .* is valid for .* but not for Fortran") # GNU
  endif()

  # Add patterns for common errors
  check_compiler_flag_common_patterns(COMPILER_FLAG_COMMON_PATTERNS)
  foreach(arg IN LISTS COMPILER_FLAG_COMMON_PATTERNS)
    if(arg MATCHES "^FAIL_REGEX$")
      continue()
    endif()
    list(APPEND CCCF_FAIL_REGEX "${arg}")
  endforeach()

  if(NOT CCCF_SRC_EXT)
    if (lang STREQUAL "C")
      set(CCCF_SRC_EXT c)
    elseif(lang STREQUAL "CXX")
      set(CCCF_SRC_EXT cxx)
    elseif(lang STREQUAL "Fortran")
      set(CCCF_SRC_EXT F)
    endif()
  endif()

  # Compute the directory in which to run the test.
  set(COMPILER_FLAG_DIR "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp")
  # Compute source and output files.
  set(COMPILER_FLAG_SRC
    "${COMPILER_FLAG_DIR}/CompilerFlag${lang}.${CCCF_SRC_EXT}")
  if(lang STREQUAL "Fortran")
    file(WRITE "${COMPILER_FLAG_SRC}"
      "      program simple\n      end program simple\n")
  else()
    file(WRITE "${COMPILER_FLAG_SRC}" "int main (void)\n{ return 0; }\n")
  endif()
  get_filename_component(COMPILER_FLAG_EXE "${COMPILER_FLAG_SRC}" NAME_WE)
  string(APPEND COMPILER_FLAG_EXE "${CMAKE_EXECUTABLE_SUFFIX}")

  # Build command line
  string(REPLACE "<SOURCE>" "${COMPILER_FLAG_SRC}" CCCF_COMMAND_PATTERN
    "${CCCF_COMMAND_PATTERN}")
  string(REPLACE "<OUTPUT>" "${COMPILER_FLAG_EXE}" CCCF_COMMAND_PATTERN
    "${CCCF_COMMAND_PATTERN}")
  string(REPLACE "<FLAG>" "${flag}" CCCF_COMMAND_PATTERN
    "${CCCF_COMMAND_PATTERN}")
  separate_arguments(CCCF_COMMAND_PATTERN UNIX_COMMAND
    "${CCCF_COMMAND_PATTERN}")

  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env LC_ALL=C LC_MESSAGES=C LANG=C
            "${CMAKE_${lang}_COMPILER}" ${CCCF_COMMAND_PATTERN}
    WORKING_DIRECTORY "${COMPILER_FLAG_DIR}"
    OUTPUT_VARIABLE COMPILER_FLAG_OUTPUT
    ERROR_VARIABLE COMPILER_FLAG_OUTPUT
    RESULT_VARIABLE COMPILER_FLAG_RESULT)

  # Record result in the cache so we can avoid re-testing every CMake run
  set(comment "Is the -no-pie linker option supported")
  if (COMPILER_FLAG_RESULT)
    set(${result} FALSE CACHE INTERNAL ${comment})
    return()
  endif()

  foreach(regex IN LISTS CCCF_FAIL_REGEX)
    if(COMPILER_FLAG_OUTPUT MATCHES "${regex}")
      set(${result} FALSE CACHE INTERNAL ${comment})
      return()
    endif()
  endforeach()

  set(${result} TRUE CACHE INTERNAL ${comment})
endfunction()
