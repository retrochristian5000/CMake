enable_language(C)

add_library(lib1 STATIC lib1.c)
target_headers(lib1
  PUBLIC HEADERS error.c
  PRIVATE HEADER_SET a HEADERS h1.h
  PUBLIC HEADER_SET b HEADERS h2.h
  INTERFACE HEADER_SET c DIRECTORY "$<1:${CMAKE_CURRENT_SOURCE_DIR}/dir>" HEADERS "$<1:dir.h>"
  )

install(TARGETS lib1 EXPORT export HEADER_SET b HEADER_SET c DESTINATION include/dir)
install(EXPORT export FILE export.cmake NAMESPACE install:: DESTINATION lib/cmake)
export(EXPORT export FILE export.cmake NAMESPACE export::)
