
enable_language(CXX C)

add_library(foo OBJECT empty.cpp empty.c)

file(GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<BOOL:$<TARGET_OBJECTS:foo>>somefile.cpp"
  CONTENT "static const char content[] = \"$<TARGET_OBJECTS:foo>\";\n"
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/input.txt"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/input.txt" "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(foo2 $<TARGET_OBJECTS:foo> "${CMAKE_CURRENT_BINARY_DIR}/1somefile.cpp" "${CMAKE_CURRENT_BINARY_DIR}/input.txt")
