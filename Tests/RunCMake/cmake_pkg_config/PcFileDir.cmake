cmake_pkg_config(EXTRACT pcfiledir)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pcfiledir.txt
"LibDirs: ${CMAKE_PKG_CONFIG_LIBDIRS}
Includes: ${CMAKE_PKG_CONFIG_INCLUDES}
")
