enable_language(C)
enable_language(CXX)

set(can_asan FALSE)
if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
  include(CheckSourceCompiles)
  set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
  check_source_compiles(C "int main(void) { return 0; }" HAVE_ASAN)
  if(HAVE_ASAN)
    set(can_asan TRUE)
  endif()
endif()

set(can_valgrind FALSE)
find_program(VALGRIND NAMES valgrind NO_CACHE)
if(VALGRIND)
  set(can_valgrind TRUE)
endif()

set(can_sh FALSE)
find_program(SH NAMES sh NO_CACHE)
if(SH)
  set(can_sh TRUE)
endif()

set(can_code_coverage FALSE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH compiler_dir)
  find_program(GCOV NAMES gcov HINTS "${compiler_dir}" NO_CACHE)
  if(GCOV)
    set(can_code_coverage TRUE)
  endif()
endif()

set(info "")
foreach(var
  can_asan
  can_valgrind
  VALGRIND
  can_sh
  SH
  can_code_coverage
  GCOV
)
  string(APPEND info "set(${var} \"${${var}}\")\n")
endforeach()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/info.cmake" "${info}")
