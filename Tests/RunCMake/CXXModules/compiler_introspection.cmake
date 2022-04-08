enable_language(CXX)

set(info "")

# Forward information about the C++ compile features.
string(APPEND info "\
set(CMAKE_CXX_COMPILE_FEATURES \"${CMAKE_CXX_COMPILE_FEATURES}\")
set(CMAKE_MAKE_PROGRAM \"${CMAKE_MAKE_PROGRAM}\")
")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/info.cmake" "${info}")
