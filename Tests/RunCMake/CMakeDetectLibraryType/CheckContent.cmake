include(CMakeDetectLibraryType)
cmake_detect_library_type(actual PATH "${path}")
if (NOT actual STREQUAL expected)
  message(FATAL_ERROR
    "${path} was detected as a ${actual} library instead of a ${expected} library")
endif ()

file(WRITE "${stamp}" "")
