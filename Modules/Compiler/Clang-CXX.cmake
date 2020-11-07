include(Compiler/Clang)
__compiler_clang(CXX)
__compiler_clang_cxx_standards(CXX)

if("x${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "xGNU")
  if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
      AND CMAKE_GENERATOR MATCHES "Makefiles"
      AND CMAKE_DEPFILE_FLAGS_CXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.0)
    # dependencies are computed by the compiler itself
    string(PREPEND CMAKE_DEPFILE_FLAGS_CXX "-MP ")
    if (CMAKE_GENERATOR MATCHES "NMake Makefiles")
      string(PREPEND CMAKE_DEPFILE_FLAGS_CXX "-MV ")
      # NMake does not manage correctly the dependency between the object and
      # the dependency file: compiler is systematically re-launched even if
      # the object is up-to-date against the dependency file.
      # So, force a newer time-stamp of the object to avoid useless re-compilation.
      set(CMAKE_CXX_DEPENDS_EXTRA_COMMANDS "@<CMAKE_COMMAND> -E touch_nocreate <OBJECT>")
      set(CMAKE_CXX_DEPFILE_FORMAT nmake)
    else()
      set(CMAKE_CXX_DEPFILE_FORMAT make)
    endif()
    set(CMAKE_CXX_DEPENDS_USE_COMPILER TRUE)
  endif()

  set(CMAKE_CXX_COMPILE_OPTIONS_VISIBILITY_INLINES_HIDDEN "-fvisibility-inlines-hidden")
endif()

cmake_policy(GET CMP0025 appleClangPolicy)
if(APPLE AND NOT appleClangPolicy STREQUAL NEW)
  return()
endif()

if("x${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "xMSVC")
  set(CMAKE_CXX_CLANG_TIDY_DRIVER_MODE "cl")
endif()
