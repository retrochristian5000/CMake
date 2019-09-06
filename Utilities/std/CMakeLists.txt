PROJECT(CMSTD)

INCLUDE_DIRECTORIES(SYSTEM "${CMSTD_SOURCE_DIR}")

# source files for CMake std library
SET(SRCS cm/bits/string_view.cxx
         cm/memory
         cm/optional
         cm/shared_mutex
         cm/string_view
         cm/utility)

ADD_LIBRARY(cmstd STATIC ${SRCS})
