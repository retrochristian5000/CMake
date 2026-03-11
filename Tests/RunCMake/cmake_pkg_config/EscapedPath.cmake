cmake_pkg_config(EXTRACT escaped-path)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/escaped-path.txt
"LibDirs: ${CMAKE_PKG_CONFIG_LIBDIRS}
Includes: ${CMAKE_PKG_CONFIG_INCLUDES}
")
