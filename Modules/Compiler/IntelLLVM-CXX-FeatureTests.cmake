# IntelLLVM compilers support all features needed to test for features
set(_cmake_oldestSupported "1")
set(_cmake_Clang34Supported "1")

include("${CMAKE_CURRENT_LIST_DIR}/Clang-CXX-TestableFeatures.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Clang-CXX-VersionedFeatures.cmake")
