include(Compiler/IBMClang)
__compiler_ibmclang(CXX)
__compiler_ibmclang_cxx_standards(CXX)

if("x${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "xGNU")
  if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
      AND CMAKE_GENERATOR MATCHES "Makefiles|WMake"
      AND CMAKE_DEPFILE_FLAGS_CXX)
    # dependencies are computed by the compiler itself
    set(CMAKE_CXX_DEPFILE_FORMAT gcc)
    set(CMAKE_CXX_DEPENDS_USE_COMPILER TRUE)
  endif()

  set(CMAKE_CXX_COMPILE_OPTIONS_EXPLICIT_LANGUAGE -x c++)
  set(CMAKE_CXX_COMPILE_OPTIONS_VISIBILITY_INLINES_HIDDEN "-fvisibility-inlines-hidden")
endif()

if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 17.1.0)
  set(CMAKE_CXX17_STANDARD_COMPILE_OPTION  "-std=c++17")
  set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++17")
endif()

__compiler_check_default_language_standard(CXX 17.1.0 17)

set(CMAKE_CXX_COMPILE_OBJECT
  "<CMAKE_CXX_COMPILER> -x c++ <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
