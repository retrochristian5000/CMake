if(NOT DEFINED _CMAKE_PROCESSING_LANGUAGE OR _CMAKE_PROCESSING_LANGUAGE STREQUAL "")
  message(FATAL_ERROR "Internal error: _CMAKE_PROCESSING_LANGUAGE is not set")
endif()

# llvm-profdata is part of the Xcode toolchain.  Prefer the one next to the
# compiler, which may belong to another Xcode than the selected one.
if(NOT CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER_LLVM_PROFDATA)
  get_filename_component(_appleclang_hint_1 "${CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER}" REALPATH)
  get_filename_component(_appleclang_hint_1 "${_appleclang_hint_1}" DIRECTORY)
  get_filename_component(_appleclang_hint_2 "${CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER}" DIRECTORY)
  find_program(_appleclang_profdata
    NAMES llvm-profdata
    HINTS "${_appleclang_hint_1}" "${_appleclang_hint_2}"
    NO_DEFAULT_PATH
    NO_CACHE
  )
  if(NOT _appleclang_profdata)
    execute_process(COMMAND xcrun --find llvm-profdata
      OUTPUT_VARIABLE _appleclang_profdata OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
      RESULT_VARIABLE _xcrun_failed)
    if(_xcrun_failed OR NOT EXISTS "${_appleclang_profdata}")
      set(_appleclang_profdata "")
    endif()
  endif()
  if(_appleclang_profdata)
    set(CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER_LLVM_PROFDATA "${_appleclang_profdata}"
      CACHE FILEPATH "LLVM profile data tool")
    mark_as_advanced(CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER_LLVM_PROFDATA)
  endif()
  unset(_appleclang_hint_1)
  unset(_appleclang_hint_2)
  unset(_appleclang_profdata)
  unset(_xcrun_failed)
endif()
