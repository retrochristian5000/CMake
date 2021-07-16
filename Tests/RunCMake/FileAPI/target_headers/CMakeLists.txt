add_library(c_headers_1 STATIC empty.c)

target_headers(c_headers_1
  PUBLIC HEADERS error.c other.c
  PRIVATE HEADER_SET a HEADERS h1.h
  )
target_headers(c_headers_1
  PUBLIC HEADER_SET b DIRECTORY "$<1:${CMAKE_CURRENT_SOURCE_DIR}/dir>" HEADERS $<1:h2.h>
  )
target_headers(c_headers_1
  INTERFACE HEADER_SET c HEADERS h3.h
  )
source_group("Source Files" FILES "${CMAKE_CURRENT_SOURCE_DIR}/other.c")

add_library(c_headers_2 STATIC empty.c)
target_headers(c_headers_2
  INTERFACE HEADERS h1.h
  )

install(TARGETS c_headers_1
  HEADERS DESTINATION include COMPONENT Headers
  HEADER_SET b DESTINATION include/dir
  HEADER_SET c
  )
install(TARGETS c_headers_2)
