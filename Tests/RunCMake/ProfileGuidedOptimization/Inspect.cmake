enable_language(C)

set(info "")
foreach(var IN ITEMS
    CMAKE_C_COMPILER_ID
    CMAKE_C_COMPILER_VERSION
    CMAKE_C_COMPILE_OPTIONS_PGO_GENERATE
    CMAKE_C_PGO_MERGE
    CMAKE_C_PGO_MERGE_OUTPUT
    )
  string(APPEND info "set(${var} [==[${${var}}]==])\n")
endforeach()

file(WRITE "${CMAKE_BINARY_DIR}/info.cmake" "${info}")
