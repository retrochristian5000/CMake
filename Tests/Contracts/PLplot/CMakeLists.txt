cmake_minimum_required(VERSION 3.9)
project(PLplotDriver NONE)
include(ExternalProject)
include(${CMAKE_CURRENT_SOURCE_DIR}/../Home.cmake)
set(plplot_prefix "${HOME}/.cmake/Contracts/PLplot")
file(REMOVE_RECURSE ${plplot_prefix})
ExternalProject_Add(PLplot
  GIT_REPOSITORY "https://git.code.sf.net/p/plplot/plplot.git"
  GIT_TAG "plplot-5.13.0"
  PREFIX ${plplot_prefix}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    ${PLplot_CMAKE_ARGS}
  )
