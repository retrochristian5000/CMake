add_custom_target(foobar
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/generated.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated.h
)

add_custom_command(
  OUTPUT generated.hpp
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/generated.h
                                   ${CMAKE_CURRENT_BINARY_DIR}/generated.hpp
  CODEGEN
)

add_custom_target(hpp_creator ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated.hpp)

# This test will fail if add_dependencies isn't account for in the
# codegen build graph
add_dependencies(hpp_creator foobar)

# Verify generated.hpp was created
add_test(NAME compare
    COMMAND ${CMAKE_COMMAND} -E compare_files
        ${CMAKE_CURRENT_SOURCE_DIR}/generated.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated.hpp
)
