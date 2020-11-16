include(Compiler/Clang)
__compiler_clang(OBJC)

if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
    AND CMAKE_GENERATOR MATCHES "Makefiles|WMake"
    AND CMAKE_DEPFILE_FLAGS_OBJC AND NOT CMAKE_OBJC_COMPILER_VERSION VERSION_LESS 3.0)
  # dependencies are computed by the compiler itself
  string(PREPEND CMAKE_DEPFILE_FLAGS_OBJC "-MP ")
  if (CMAKE_GENERATOR MATCHES "NMake Makefiles")
    string(PREPEND CMAKE_DEPFILE_FLAGS_OBJC "-MV ")
    # NMake does not manage correctly the dependency between the object and
    # the dependency file: compiler is systematically re-launched even if
    # the object is up-to-date against the dependency file.
    # So, force a newer time-stamp of the object to avoid useless re-compilation.
    set(CMAKE_OBJC_DEPENDS_EXTRA_COMMANDS "@<CMAKE_COMMAND> -E touch_nocreate <OBJECT>")
    set(CMAKE_OBJC_DEPFILE_FORMAT nmake)
  else()
    set(CMAKE_OBJC_DEPFILE_FORMAT make)
  endif()
  set(CMAKE_OBJC_DEPENDS_USE_COMPILER TRUE)
endif()


if(NOT CMAKE_OBJC_COMPILER_VERSION VERSION_LESS 3.4)
  set(CMAKE_OBJC90_STANDARD_COMPILE_OPTION "-std=c90")
  set(CMAKE_OBJC90_EXTENSION_COMPILE_OPTION "-std=gnu90")
  set(CMAKE_OBJC90_STANDARD__HAS_FULL_SUPPORT ON)

  set(CMAKE_OBJC99_STANDARD_COMPILE_OPTION "-std=c99")
  set(CMAKE_OBJC99_EXTENSION_COMPILE_OPTION "-std=gnu99")
  set(CMAKE_OBJC99_STANDARD__HAS_FULL_SUPPORT ON)

  set(CMAKE_OBJC11_STANDARD_COMPILE_OPTION "-std=c11")
  set(CMAKE_OBJC11_EXTENSION_COMPILE_OPTION "-std=gnu11")
  set(CMAKE_OBJC11_STANDARD__HAS_FULL_SUPPORT ON)
endif()

__compiler_check_default_language_standard(OBJC 3.4 99 3.6 11)
