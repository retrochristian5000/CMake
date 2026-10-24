include(Compiler/Clang-CXX)
include(Compiler/EMBTClang)

# The toolchain's clang-scan-deps does not apply the bcc64x driver's target,
# standard, and include paths, so C++ module scanning is not supported.
unset(CMAKE_CXX_SCANDEP_SOURCE)

string(APPEND CMAKE_CXX_FLAGS_INIT " -q")
