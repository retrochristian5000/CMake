# A C++20 target is scanned for module dependencies with clang-scan-deps, which
# not every installation provides.  Build one with the scanner as found beside
# bcc64x, and again with it forced absent: both must build, and the absent case
# must not scan (an empty scanner command would fail the build).  With Ninja,
# check build.ninja for scan steps.  Driver-gated: a no-op where bcc64x is not
# installed.
if(NOT WIN32)
  return()
endif()

set(_hints "")
if(DEFINED ENV{TP})
  list(APPEND _hints "$ENV{TP}/bin64" "$ENV{TP}/bin")
endif()
find_program(EMBT_BCC64X NAMES bcc64x HINTS ${_hints})
if(NOT EMBT_BCC64X)
  message(STATUS "bcc64x not found; C++ module scanning check skipped.")
  return()
endif()

foreach(_case found absent)
  set(_b "${CMAKE_CURRENT_BINARY_DIR}/scan-${_case}")
  file(REMOVE_RECURSE "${_b}")
  set(_opts "")
  if(_case STREQUAL "absent")
    # An empty cache value is kept by find_program, so no scanner is used.
    set(_opts "-DCMAKE_CXX_COMPILER_CLANG_SCAN_DEPS=")
  endif()
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      -S "${CMAKE_CURRENT_SOURCE_DIR}/cxx-module-scanning"
      -B "${_b}"
      -G "${CMAKE_GENERATOR}"
      "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
      "-DCMAKE_CXX_COMPILER=${EMBT_BCC64X}"
      ${_opts}
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err RESULT_VARIABLE _rc)
  if(NOT _rc EQUAL 0)
    message(SEND_ERROR "configure (${_case}) failed:\n${_out}${_err}")
    continue()
  endif()
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build "${_b}"
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err RESULT_VARIABLE _rc)
  if(NOT _rc EQUAL 0)
    message(SEND_ERROR "build (${_case}) failed:\n${_out}${_err}")
    continue()
  endif()

  if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    continue()
  endif()
  file(STRINGS "${_b}/CMakeCache.txt" _scanner
    REGEX "^CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS:")
  string(REGEX REPLACE "^[^=]*=" "" _scanner "${_scanner}")
  file(GLOB _ninja_files "${_b}/build*.ninja")
  set(_scan_steps "")
  foreach(_f IN LISTS _ninja_files)
    file(STRINGS "${_f}" _steps REGEX "^build [^:]*: CXX_SCAN__")
    list(APPEND _scan_steps ${_steps})
  endforeach()
  if(_scanner AND NOT _scan_steps)
    message(SEND_ERROR
      "clang-scan-deps was found (${_scanner}) but nothing is scanned.")
  elseif(NOT _scanner AND _scan_steps)
    message(SEND_ERROR
      "clang-scan-deps is absent (${_case}) but sources are scanned:\n"
      "${_scan_steps}")
  endif()
endforeach()
