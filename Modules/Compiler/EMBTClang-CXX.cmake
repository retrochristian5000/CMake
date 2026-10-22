include(Compiler/Clang-CXX)
include(Compiler/EMBTClang)

# Clang-CXX enables C++ module scanning via clang-scan-deps.  That tool is not
# present in every installation; without it CMake would emit an empty scan
# command that fails every C++20 build, so disable scanning when it is missing.
if(NOT CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

string(APPEND CMAKE_CXX_FLAGS_INIT " -q")
