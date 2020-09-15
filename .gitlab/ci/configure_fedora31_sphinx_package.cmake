# Disable formats not wanted in the package's documentation.
set(SPHINX_INFO OFF CACHE BOOL "")
set(SPHINX_SINGLEHTML OFF CACHE BOOL "")
set(SPHINX_TEXT OFF CACHE BOOL "")

# Set the destination directory for docs that packages expect.
set(CMAKE_DOC_DIR "doc/cmake" CACHE STRING "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_sphinx.cmake")
