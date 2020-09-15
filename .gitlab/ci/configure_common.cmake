set(CTEST_USE_LAUNCHERS "ON" CACHE BOOL "")

# We run the install right after the build. Avoid rerunning it when installing.
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY "ON" CACHE BOOL "")
# Install CMake under the build tree.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "")
set(CMake_TEST_INSTALL "OFF" CACHE BOOL "")

# Qt binaries get placed inside the source directory, which causes them to not
# be included in the install-time rpath, but we still want them in the
# build-time rpath. CMake sets CMAKE_BUILD_WITH_INSTALL_RPATH to ON by default,
# so set it to OFF.
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF CACHE BOOL "")

if (NOT "$ENV{CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "$ENV{CMAKE_BUILD_TYPE}" CACHE STRING "")
endif ()

include("${CMAKE_CURRENT_LIST_DIR}/configure_sccache.cmake")
