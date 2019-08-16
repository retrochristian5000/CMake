cmake_minimum_required(VERSION 3.1)

project(TestRequiredArtifacts.Check LANGUAGES C)

set (components)
if (CHECK_INTERPRETER)
  list (APPEND components Interpreter)
endif()
if (CHECK_LIBRARY OR CHECK_INCLUDE)
  list (APPEND components Development)
endif()

find_package (Python3 COMPONENTS ${components})


if (PYTHON_IS_FOUND AND NOT Python3_FOUND)
  message (FATAL_ERROR "Python3 unexpectedly not found")
endif()
if (NOT PYTHON_IS_FOUND AND Python3_FOUND)
  message (FATAL_ERROR "Python3 unexpectedly found")
endif()


if (CHECK_INTERPRETER AND NOT Python3_EXECUTABLE STREQUAL Python3_REQUIRED_INTERPRETER)
  message (FATAL_ERROR "Fail to use variable Python3_REQUIRED_INTERPRETER")
endif()

if (CHECK_LIBRARY AND NOT Python3_LIBRARY_RELEASE STREQUAL Python3_REQUIRED_LIBRARY)
  message (FATAL_ERROR "Fail to use variable Python3_REQUIRED_LIBRARY")
endif()

if (CHECK_INCLUDE AND NOT Python3_INCLUDE_DIR STREQUAL Python3_REQUIRED_INCLUDE_DIR)
  message (FATAL_ERROR "Fail to use variable Python3_REQUIRED_INCLUDE_DIR")
endif()
