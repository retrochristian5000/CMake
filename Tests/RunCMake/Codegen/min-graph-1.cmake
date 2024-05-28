add_executable(foobar main.c)

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/generated.h
  COMMAND
    ${CMAKE_COMMAND} -E
        copy ${CMAKE_CURRENT_SOURCE_DIR}/generated.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated.h
  COMMAND
    # Generate a header file that requires foobar
    foobar
  CODEGEN
)

add_library(errorlib
  # If this library is built error.c will cause the build to fail
  error.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated.h
)

enable_testing()

# Ensure code generation worked by verifying generated.h was created
add_test(NAME compare
    COMMAND ${CMAKE_COMMAND} -E compare_files
        ${CMAKE_CURRENT_SOURCE_DIR}/generated.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated.h
)

# foobar should be built since it was needed
# by the code generation
add_test(NAME ensure_exists COMMAND foobar)
