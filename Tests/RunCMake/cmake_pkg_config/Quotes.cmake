cmake_pkg_config(EXTRACT quotes)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/quotes.txt
"LibDirs: ${CMAKE_PKG_CONFIG_LIBDIRS}
Includes: ${CMAKE_PKG_CONFIG_INCLUDES}
")
