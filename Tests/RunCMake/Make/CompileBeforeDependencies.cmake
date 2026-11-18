enable_language(C)

add_library(dep SHARED dep.c)
add_library(main SHARED main.c)
target_link_libraries(main PRIVATE dep)

# A non-CODEGEN generated header consumed through a link edge: the
# consumer's compile must wait for the dependency's custom command
# outputs, but not for the dependency's link.
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated.h"
  # Sleep to exercise the ordering issue.
  COMMAND "${CMAKE_COMMAND}" -E sleep 2
  COMMAND "${CMAKE_COMMAND}" -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/generated.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated.h"
  VERBATIM
)

add_library(gen_dep STATIC
  gen_dep.c
  "${CMAKE_CURRENT_BINARY_DIR}/generated.h"
)
target_include_directories(gen_dep PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

add_library(gen_main STATIC gen_main.c)
target_link_libraries(gen_main PRIVATE gen_dep)
