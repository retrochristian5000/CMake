cmake_minimum_required (VERSION 3.14)

if (IOS)
  set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO)
  set (CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif ()

enable_language (Swift)
enable_language (CXX)

set (CMAKE_Swift_LANGUAGE_VERSION 5.0)

find_package(ZLIB REQUIRED)
add_library (framework_dependency STATIC)
target_sources (framework_dependency PRIVATE use_cmath.cpp)
target_link_libraries(framework_dependency INTERFACE ZLIB::ZLIB)

add_library (framework_test SHARED use_cmath.cpp foo.swift)
target_link_libraries (framework_test PRIVATE framework_dependency)

set_target_properties (framework_test PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION A
    MACOSX_FRAMEWORK_IDENTIFIER "framework.test"
    VERSION "1.0"
    SOVERSION 1.0
  )
