add_custom_target(target ALL)
set_property(TARGET target PROPERTY COMPILE_DEFINITIONS "touched")

add_custom_command(
  TARGET target
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/$<TARGET_PROPERTY:COMPILE_DEFINITIONS>
)
