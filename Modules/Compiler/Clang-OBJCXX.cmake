include(Compiler/Clang)
__compiler_clang(OBJCXX)
__compiler_clang_cxx_standards(OBJCXX)

if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
    AND CMAKE_GENERATOR MATCHES "Makefiles"
    AND CMAKE_DEPFILE_FLAGS_OBJCXX AND NOT CMAKE_OBJCXX_COMPILER_VERSION VERSION_LESS 3.0)
  # dependencies are computed by the compiler itself
  string(PREPEND CMAKE_DEPFILE_FLAGS_OBJCXX "-MP ")
    if (CMAKE_GENERATOR MATCHES "NMake Makefiles")
      string(PREPEND CMAKE_DEPFILE_FLAGS_OBJCXX "-MV ")
      # NMake does not manage correctly the dependency between the object and
      # the dependency file: compiler is systematically re-launched even if
      # the object is up-to-date against the dependency file.
      # So, force a newer time-stamp of the object to avoid useless re-compilation.
      set(CMAKE_OBJCXX_DEPENDS_EXTRA_COMMANDS "@<CMAKE_COMMAND> -E touch_nocreate <OBJECT>")
    endif()
  set(CMAKE_OBJCXX_DEPENDS_USE_COMPILER TRUE)
endif()
