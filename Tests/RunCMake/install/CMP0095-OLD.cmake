cmake_policy(SET CMP0095 OLD)
project(cmp0095-old)

add_executable(exe main.c)
set_property(TARGET exe PROPERTY INSTALL_RPATH "\\\${ORIGIN}/../lib")

add_executable(exe2 main.c)
set_property(TARGET exe2 PROPERTY INSTALL_RPATH "/opt/foobar/lib")

add_executable(exe3 main.c)
set_property(TARGET exe3 PROPERTY INSTALL_RPATH "$ORIGIN/../lib")

add_library(lib1 SHARED obj1.c)
add_executable(exe4 main.c)
target_link_libraries(exe4 PRIVATE lib1)
set_target_properties(exe4 PROPERTIES
  BUILD_RPATH_USE_ORIGIN ON
  INSTALL_RPATH "\\\${ORIGIN}/../foolib")

install(TARGETS exe exe2 exe3 lib1 exe4)
