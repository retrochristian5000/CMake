cmake_minimum_required(VERSION 3.10) # just lower than 3.15
project(cmp0095-warn)

# uses cmake syntax, should issue an author warning
add_executable(exe main.c)
set_property(TARGET exe PROPERTY INSTALL_RPATH "\\\${ORIGIN}/../lib")

# no cmake syntax used, should not warn
add_executable(exe2 main.c)
set_property(TARGET exe2 PROPERTY INSTALL_RPATH "/opt/foobar/lib")

# this already worked without CMP0095, should not warn
add_executable(exe3 main.c)
set_property(TARGET exe3 PROPERTY INSTALL_RPATH "$ORIGIN/../lib")

# This test case hopefully catches potentially breaking changes in the
# BUILD_RPATH_USE_ORIGIN behaviour. Besides, it should issue an author
# warning for the INSTALL_RPATH property.
add_library(lib1 SHARED obj1.c)
add_executable(exe4 main.c)
target_link_libraries(exe4 PRIVATE lib1)
set_target_properties(exe4 PROPERTIES
  BUILD_RPATH_USE_ORIGIN ON
  INSTALL_RPATH "\\\${ORIGIN}/../foolib")

install(TARGETS exe exe2 exe3 lib1 exe4)
