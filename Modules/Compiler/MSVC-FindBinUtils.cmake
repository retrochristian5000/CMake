if(NOT DEFINED _CMAKE_PROCESSING_LANGUAGE OR _CMAKE_PROCESSING_LANGUAGE STREQUAL "")
  message(FATAL_ERROR "Internal error: _CMAKE_PROCESSING_LANGUAGE is not set")
endif()

# pgomgr merges the profile guided optimization data.  Use only the one of
# the compiler toolset, which matches the linker writing the '.pgd' files.
get_filename_component(_msvc_pgomgr_hint
  "${CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER}" DIRECTORY)
find_program(CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER_PGOMGR
  NAMES pgomgr
  PATHS "${_msvc_pgomgr_hint}"
  NO_DEFAULT_PATH
  NO_CACHE
)
if(NOT CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER_PGOMGR)
  set(CMAKE_${_CMAKE_PROCESSING_LANGUAGE}_COMPILER_PGOMGR "")
endif()
unset(_msvc_pgomgr_hint)
