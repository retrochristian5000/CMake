project(ProjectA LANGUAGES C)

file(WRITE
  "${CMAKE_CURRENT_BINARY_DIR}/main.cxx"
  "int main() { return 0; }
"
  )

add_executable(UsesCXXLang
  "${CMAKE_CURRENT_BINARY_DIR}/main.cxx"
  )

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/main.cxx" PROPERTIES LANGUAGE CXX)