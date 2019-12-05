add_library(empty ../empty.c)
target_precompile_headers(empty PRIVATE
  <stdio.h>
  <string.h>
)
target_include_directories(empty PUBLIC ../include)
