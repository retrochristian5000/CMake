enable_language(C)

add_library(lib INTERFACE)
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/dependency.h
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/dependency.h
  VERBATIM
  )
target_sources(lib PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_BINARY_DIR} FILES ${CMAKE_BINARY_DIR}/dependency.h)

add_executable(exe dependency.c)
target_link_libraries(exe PRIVATE lib)
