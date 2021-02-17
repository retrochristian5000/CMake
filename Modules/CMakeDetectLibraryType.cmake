# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
CMakeDetectLibraryType
----------------------

Function to determine the library type of a given path.

This function classifies a library file path as a ``SHARED``, ``STATIC``,
``MODULE``, or ``UNKNOWN`` library. Note that on many platforms, ``MODULE``
libraries will actually be detected as ``SHARED`` libraries due to shared
extensions. This can be used to classify libraries for the currently active
platform. That is, it will not discern Windows library differences on a Linux
host machine.

At least one language with shared and static libraries must have been enabled
for CMake to detect platform information that is used by this module.

Usage:

.. code-block:: cmake

  cmake_detect_library_type(<variable> PATH <path>)

Where ``<variable>`` is the library type suitable for use in
:command:`add_library` for an ``IMPORTED`` target:

.. code-block:: cmake

  cmake_detect_library_type(_findpkg_type PATH "${Target_LIBRARY}")
  add_library(FindPkg::Target "${_findpkg_type}" IMPORTED)

#]=======================================================================]
function (cmake_detect_library_type output)
  cmake_parse_arguments(PARSE_ARGV 1 cdlt
    ""
    "PATH"
    "")

  if (NOT DEFINED cdlt_PATH)
    message(FATAL_ERROR
      "The `PATH` argument is required.")
  endif ()

  if (DEFINED cdlt_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "Unparsed arguments for cmake_detect_library_type: "
      "${cdlt_UNPARSED_ARGUMENTS}")
  endif ()

  if (NOT cdlt_PATH)
    message(FATAL_ERROR
      "The `PATH` argument is empty.")
  endif ()

  set(cdlt_type UNKNOWN)
  # Windows libraries all end with `.lib`. We need to detect the type based on
  # the contents of the library. However, MinGW does use different extensions.
  if (WIN32 AND NOT MINGW AND
      DEFINED CMAKE_IMPORT_LIBRARY_SUFFIX AND
      cdlt_PATH MATCHES "${CMAKE_IMPORT_LIBRARY_SUFFIX}$")
    find_program(CDLT_DUMPBIN_EXECUTABLE
      NAMES dumpbin
      DOC   "Path to the dumpbin executable")
    mark_as_advanced(CDLT_DUMPBIN_EXECUTABLE)
    execute_process(
      COMMAND "${CDLT_DUMPBIN_EXECUTABLE}"
              /HEADERS
              "${cdlt_PATH}"
      OUTPUT_VARIABLE cdlt_out
      ERROR_VARIABLE  cdlt_err
      RESULT_VARIABLE cdlt_res)
    if (cdlt_res)
      message(WARNING
        "Failed to run `dumpbin` on ${cdlt_PATH}. Cannot determine "
        "shared/static library type: ${cdlt_err}")
    else ()
      if (cdlt_out MATCHES "DLL name     :")
        set(cdlt_type SHARED)
      else ()
        set(cdlt_type STATIC)
      endif ()
    endif ()
  else ()
    string(LENGTH "${cdlt_PATH}" cdlt_path_len)

    string(LENGTH "${CMAKE_SHARED_LIBRARY_SUFFIX}" cdlt_shared_suffix_len)
    math(EXPR cdlt_shared_idx "${cdlt_path_len} - ${cdlt_shared_suffix_len}")
    string(SUBSTRING "${cdlt_PATH}" "${cdlt_shared_idx}" -1 cdlt_shared_check)

    string(LENGTH "${CMAKE_STATIC_LIBRARY_SUFFIX}" cdlt_static_suffix_len)
    math(EXPR cdlt_static_idx "${cdlt_path_len} - ${cdlt_static_suffix_len}")
    string(SUBSTRING "${cdlt_PATH}" "${cdlt_static_idx}" -1 cdlt_static_check)

    string(LENGTH "${CMAKE_SHARED_MODULE_SUFFIX}" cdlt_module_suffix_len)
    math(EXPR cdlt_module_idx "${cdlt_path_len} - ${cdlt_module_suffix_len}")
    string(SUBSTRING "${cdlt_PATH}" "${cdlt_module_idx}" -1 cdlt_module_check)

    if (cdlt_shared_check STREQUAL CMAKE_SHARED_LIBRARY_SUFFIX)
      set(cdlt_type SHARED)
    elseif (cdlt_static_check STREQUAL CMAKE_STATIC_LIBRARY_SUFFIX)
      set(cdlt_type STATIC)
    elseif (cdlt_module_check STREQUAL CMAKE_SHARED_MODULE_SUFFIX)
      set(cdlt_type MODULE)
    endif ()

    # when import suffix != static suffix, we can disambiguate static and import
    if (WIN32 AND NOT CMAKE_IMPORT_LIBRARY_SUFFIX STREQUAL CMAKE_STATIC_LIBRARY_SUFFIX)
      string(LENGTH "${CMAKE_IMPORT_LIBRARY_SUFFIX}" cdlt_import_suffix_len)
      math(EXPR cdlt_import_idx "${cdlt_path_len} - ${cdlt_import_suffix_len}")
      string(SUBSTRING "${cdlt_PATH}" "${cdlt_import_idx}" -1 cdlt_import_check)
      if (cdlt_import_check STREQUAL CMAKE_IMPORT_LIBRARY_SUFFIX)
        set(cdlt_type SHARED)
      endif ()
    endif ()
  endif ()

  set("${output}"
    "${cdlt_type}"
    PARENT_SCOPE)
endfunction ()
