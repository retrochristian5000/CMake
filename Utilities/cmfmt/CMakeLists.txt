project(fmt CXX)

set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "")
set(CMAKE_CXX_CLANG_TIDY "")

add_library(cmfmt STATIC
  src/format.cc
  include/fmt/core.h
  include/fmt/format.h
  )

target_include_directories(cmfmt PUBLIC include)

install(FILES LICENSE.rst DESTINATION ${CMAKE_DOC_DIR}/cmfmt)
