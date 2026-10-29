# Show that the compiler warning about a deprecated symbol includes the
# message given to the deprecation macros.  The objects are only compiled, so
# the symbols need not be exported, and each is built in a separate step that
# checks the warnings.
add_library(deprecated_msg_libshared OBJECT EXCLUDE_FROM_ALL libshared.cpp)

add_library(deprecated_msg_libattributes OBJECT EXCLUDE_FROM_ALL
  libattributes.cpp)
set_property(TARGET deprecated_msg_libattributes PROPERTY CXX_STANDARD 14)

set(targets deprecated_msg_libshared deprecated_msg_libattributes)

# C++11 consumers get the GNU spelling of the deprecation attribute.
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
  add_library(deprecated_msg_libattributes_cxx11 OBJECT EXCLUDE_FROM_ALL
    libattributes.cpp)
  set_property(TARGET deprecated_msg_libattributes_cxx11
    PROPERTY CXX_STANDARD 11)
  list(APPEND targets deprecated_msg_libattributes_cxx11)
endif()

if(NOT COMPILER_HAS_DEPRECATED)
  set(targets "")
endif()
list(JOIN targets "\n" targets)
file(WRITE ${CMAKE_BINARY_DIR}/deprecated_msg_targets "${targets}")
