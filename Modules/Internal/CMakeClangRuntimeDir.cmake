# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

# Get the runtime library directory of the 'clang-cl' compiler for the given
# language in 'out_var', or an empty string if it cannot be determined.
# MSVC's 'link.exe' needs it, e.g. for profile guided optimization.
function(cmake_clang_cl_runtime_dir lang out_var)
  set(${out_var} "" PARENT_SCOPE)
  # Use the compiler arguments as split to determine the compiler, or else,
  # e.g. when called by the platform modules, split them the same way.
  if (DEFINED CMAKE_${lang}_COMPILER_ID_ARG1)
    set(_clang_arg1 ${CMAKE_${lang}_COMPILER_ID_ARG1})
  else ()
    string(STRIP "${CMAKE_${lang}_COMPILER_ARG1}" _clang_arg1)
    string(REGEX REPLACE " +" ";" _clang_arg1 "${_clang_arg1}")
  endif ()
  # The target may also be selected by the user flags, e.g. '-m32', or by
  # the Visual Studio platform, so use the detected architecture.
  set(_clang_target "")
  if (CMAKE_${lang}_COMPILER_TARGET)
    set(_clang_target "--target=${CMAKE_${lang}_COMPILER_TARGET}")
  elseif ("x${CMAKE_${lang}_COMPILER_ARCHITECTURE_ID}" STREQUAL "xX86")
    set(_clang_target "--target=i686-pc-windows-msvc")
  elseif ("x${CMAKE_${lang}_COMPILER_ARCHITECTURE_ID}" STREQUAL "xx64")
    set(_clang_target "--target=x86_64-pc-windows-msvc")
  elseif ("x${CMAKE_${lang}_COMPILER_ARCHITECTURE_ID}" STREQUAL "xARM64")
    set(_clang_target "--target=aarch64-pc-windows-msvc")
  elseif ("x${CMAKE_${lang}_COMPILER_ARCHITECTURE_ID}" STREQUAL "xARM64EC")
    set(_clang_target "--target=arm64ec-pc-windows-msvc")
  elseif ("x${CMAKE_${lang}_COMPILER_ARCHITECTURE_ID}" STREQUAL "xARMV7")
    set(_clang_target "--target=thumbv7-pc-windows-msvc")
  endif ()
  execute_process(
    COMMAND "${CMAKE_${lang}_COMPILER}"
      ${_clang_arg1}
      ${_clang_target}
      -clang:-print-runtime-dir
    OUTPUT_VARIABLE _clang_runtime_dir_out
    ERROR_VARIABLE _clang_runtime_dir_err
    RESULT_VARIABLE _clang_runtime_dir_res
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE)
  if (_clang_runtime_dir_res EQUAL 0)
    file(TO_CMAKE_PATH "${_clang_runtime_dir_out}" _clang_runtime_dir_out)
    if(IS_DIRECTORY "${_clang_runtime_dir_out}")
      set(${out_var} "${_clang_runtime_dir_out}" PARENT_SCOPE)
    endif()
  endif ()
endfunction()
