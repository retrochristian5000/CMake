# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.


# This module is shared by multiple languages; use include blocker.
if(__WINDOWS_CLANG)
  return()
endif()
set(__WINDOWS_CLANG 1)

macro(__windows_compiler_clang_gnu lang)
  set(CMAKE_LIBRARY_PATH_FLAG "-L")
  set(CMAKE_LINK_LIBRARY_FLAG "-l")

  set(CMAKE_IMPORT_LIBRARY_PREFIX "")
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set(CMAKE_SHARED_MODULE_PREFIX  "")
  set(CMAKE_STATIC_LIBRARY_PREFIX "")
  set(CMAKE_EXECUTABLE_SUFFIX     ".exe")
  set(CMAKE_IMPORT_LIBRARY_SUFFIX ".lib")
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
  set(CMAKE_SHARED_MODULE_SUFFIX  ".dll")
  set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
  set(CMAKE_DEPFILE_FLAGS_${lang} "-MD -MT <OBJECT> -MF <DEPFILE>")

  set(CMAKE_FIND_LIBRARY_PREFIXES "lib" "")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ".dll.a" ".a" ".lib")
  set(CMAKE_SUPPORT_WINDOWS_EXPORT_ALL_SYMBOLS 1)
  set (CMAKE_LINK_DEF_FILE_FLAG "-Xlinker /DEF:")

  # No -fPIC on Windows
  set(CMAKE_${lang}_COMPILE_OPTIONS_PIC "")
  set(CMAKE_${lang}_COMPILE_OPTIONS_PIE "")
  set(_CMAKE_${lang}_PIE_MAY_BE_SUPPORTED_BY_LINKER NO)
  set(CMAKE_${lang}_LINK_OPTIONS_PIE "")
  set(CMAKE_${lang}_LINK_OPTIONS_NO_PIE "")
  set(CMAKE_SHARED_LIBRARY_${lang}_FLAGS "")

  set(CMAKE_${lang}_USE_RESPONSE_FILE_FOR_OBJECTS 1)
  set(CMAKE_${lang}_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
  set(CMAKE_${lang}_USE_RESPONSE_FILE_FOR_INCLUDES 1)

  # Create archiving rules to support large object file lists for static libraries.
  set(CMAKE_${lang}_ARCHIVE_CREATE "<CMAKE_AR> qc <TARGET> <LINK_FLAGS> <OBJECTS>")
  set(CMAKE_${lang}_ARCHIVE_APPEND "<CMAKE_AR> q  <TARGET> <LINK_FLAGS> <OBJECTS>")
  set(CMAKE_${lang}_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")
  set(CMAKE_${lang}_CREATE_SHARED_LIBRARY
    "<CMAKE_${lang}_COMPILER> <CMAKE_SHARED_LIBRARY_${lang}_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_${lang}_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} -Xlinker /implib:<TARGET_IMPLIB> -Xlinker /pdb:<TARGET_PDB> -Xlinker /version:<TARGET_VERSION_MAJOR>.<TARGET_VERSION_MINOR> <OBJECTS> <LINK_LIBRARIES>")
  set(CMAKE_${lang}_CREATE_SHARED_MODULE ${CMAKE_${lang}_CREATE_SHARED_LIBRARY})
  set(CMAKE_${lang}_LINK_EXECUTABLE
    "<CMAKE_${lang}_COMPILER> <FLAGS> <CMAKE_${lang}_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> -Xlinker /implib:<TARGET_IMPLIB> -Xlinker /pdb:<TARGET_PDB> -Xlinker /version:<TARGET_VERSION_MAJOR>.<TARGET_VERSION_MINOR> ${CMAKE_GNULD_IMAGE_VERSION} <LINK_LIBRARIES>")

  string(APPEND CMAKE_${lang}_FLAGS_INIT " -Xclang -flto-visibility-public-std")
  string(APPEND CMAKE_${lang}_FLAGS_DEBUG_INIT " -g -Xclang -gcodeview -O0")
  string(APPEND CMAKE_${lang}_FLAGS_MINSIZEREL_INIT " -Os -DNDEBUG")
  string(APPEND CMAKE_${lang}_FLAGS_RELEASE_INIT " -O3 -DNDEBUG")
  string(APPEND CMAKE_${lang}_FLAGS_RELWITHDEBINFO_INIT " -O2 -g -DNDEBUG -Xclang -gcodeview")
  set(CMAKE_INCLUDE_SYSTEM_FLAG_${lang} "-isystem ")

  if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    set(_CRT " -Xlinker -nodefaultlib:libcmt -lucrtd -lmsvcrtd -lvcruntimed ")
  else()
    set(_CRT " -Xlinker -nodefaultlib:libcmt -lucrt -lmsvcrt -lvcruntime ")
  endif()
  set(CMAKE_${lang}_STANDARD_LIBRARIES_INIT "-lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 ${_CRT} -loldnames")

  enable_language(RC)
endmacro()

if("x${CMAKE_C_SIMULATE_ID}" STREQUAL "xMSVC"
    OR "x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC")

  if ( DEFINED CMAKE_C_COMPILER_ID
       AND DEFINED CMAKE_CXX_COMPILER_ID
       AND NOT CMAKE_C_COMPILER_ID STREQUAL "Clang"
       OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
       )
    message(FATAL_ERROR "The current configuration mixes Clang and MSVC or "
            "some other CL compatible compiler tool. This is not supported. "
            "Use either clang or MSVC as both C and C++ compilers.")
  endif()

  if ( DEFINED CMAKE_C_COMPILER_FRONTEND_VARIANT
       AND DEFINED CMAKE_CXX_COMPILER_FRONTEND_VARIANT
       AND NOT "x${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL
       "x${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}")
    message(FATAL_ERROR "The current configuration uses the Clang compiler "
            "tool with mixed frontend variants, both the GNU and in MSVC CL "
            "like variants. This is not supported. Use either clang/clang++ "
            "or clang-cl as both C and C++ compilers.")
  endif()

  if ( "x${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "xMSVC" OR "x${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "xMSVC" )
    include(Platform/Windows-MSVC)

    macro(__windows_compiler_clang lang)
      set(_COMPILE_${lang} "${_COMPILE_${lang}_MSVC}")
      __windows_compiler_msvc(${lang})
    endmacro()
  else()
    macro(__windows_compiler_clang lang)
      __windows_compiler_clang_gnu(${lang})
    endmacro()
  endif()

else()
  include(Platform/Windows-GNU)
  macro(__windows_compiler_clang lang)
    __windows_compiler_gnu(${lang})
  endmacro()
endif()
