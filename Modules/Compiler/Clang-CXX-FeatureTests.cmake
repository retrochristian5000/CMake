
# Reference: http://clang.llvm.org/cxx_status.html
# http://clang.llvm.org/docs/LanguageExtensions.html

set(_cmake_oldestSupported "((__clang_major__ * 100) + __clang_minor__) >= 301")
set(_cmake_Clang34Supported "((__clang_major__ * 100) + __clang_minor__) >= 304")

include("${CMAKE_CURRENT_LIST_DIR}/Clang-CXX-TestableFeatures.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Clang-CXX-VersionedFeatures.cmake")
