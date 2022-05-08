# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindVulkan
----------

.. versionadded:: 3.7

Find Vulkan, which is a low-overhead, cross-platform 3D graphics
and computing API.

Optional COMPONENTS
^^^^^^^^^^^^^^^^^^^

.. versionadded:: 3.XX

This module respects several optional COMPONENTS: ``glslang``, and
``shaderc_combined``.  There are corresponding import targets for each of these flags.

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` targets if Vulkan has been found:

``Vulkan::Vulkan``
  The main Vulkan library.

``Vulkan::glslc``
  .. versionadded:: 3.19

  The GLSLC SPIR-V compiler, if it has been found.

``Vulkan::Headers``
  .. versionadded:: 3.21

  Provides just Vulkan headers include paths, if found.  No library is
  included in this target.  This can be useful for applications that
  load Vulkan library dynamically.

``Vulkan::glslangValidator``
  .. versionadded:: 3.21

  The glslangValidator tool, if found.  It is used to compile GLSL and
  HLSL shaders into SPIR-V.

.. versionadded:: 3.XX
  Additionally, the following Vulkan SDK library target are defined:

``Vulkan::glslang``
  Defined if SDK has the Khronos-reference front-end shader parser and SPIR-V generator library (glslang).

``Vulkan::shaderc_combined``
  Defined if SDK has the Google static library for Vulkan shader compilation (shaderc_combined).

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``Vulkan_FOUND``
  set to true if Vulkan was found
``Vulkan_INCLUDE_DIRS``
  include directories for Vulkan
``Vulkan_LIBRARIES``
  link against this library to use Vulkan
``Vulkan_VERSION``
  .. versionadded:: 3.23

  value from ``vulkan/vulkan_core.h``
``Vulkan_glslang_FOUND``
  True, if the SDK has the glslang library.
``Vulkan_shaderc_combined_FOUND``
  True, if the SDK has the shaderc_combined library.

.. versionadded:: 3.XX
  Variables for component library ``glslang`` and ``shaderc_combined``.

The module will also defines these cache variables:

``Vulkan_INCLUDE_DIR``
  the Vulkan include directory
``Vulkan_LIBRARY``
  the path to the Vulkan library
``Vulkan_GLSLC_EXECUTABLE``
  the path to the GLSL SPIR-V compiler
``Vulkan_GLSLANG_VALIDATOR_EXECUTABLE``
  the path to the glslangValidator tool
``Vulkan_glslang_LIBRARY``
 Path to the glslang library.
``Vulkan_shaderc_combined_LIBRARY``
 Path to the shaderc_combined library.

.. versionadded:: 3.XX
  Variables for component library ``glslang`` and ``shaderc_combined``.

Hints
^^^^^

.. versionadded:: 3.18

The ``VULKAN_SDK`` environment variable optionally specifies the
location of the Vulkan SDK root directory for the given
architecture. It is typically set by sourcing the toplevel
``setup-env.sh`` script of the Vulkan SDK directory into the shell
environment.

#]=======================================================================]

if(WIN32)
  set(_Vulkan_hint_include_search_paths
    "$ENV{VULKAN_SDK}/Include"
  )
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_Vulkan_hint_executable_search_paths
      "$ENV{VULKAN_SDK}/Bin"
    )
    set(_Vulkan_hint_library_search_paths
      "$ENV{VULKAN_SDK}/Lib"
      "$ENV{VULKAN_SDK}/Bin"
    )
  else()
    set(_Vulkan_hint_executable_search_paths
      "$ENV{VULKAN_SDK}/Bin32"
    )
    set(_Vulkan_hint_library_search_paths
      "$ENV{VULKAN_SDK}/Lib32"
      "$ENV{VULKAN_SDK}/Bin32"
    )
  endif()
else()
  set(_Vulkan_hint_include_search_paths
    "$ENV{VULKAN_SDK}/include"
  )
  set(_Vulkan_hint_executable_search_paths
    "$ENV{VULKAN_SDK}/bin"
  )
  set(_Vulkan_hint_library_search_paths
    "$ENV{VULKAN_SDK}/lib"
  )
endif()

find_path(Vulkan_INCLUDE_DIR
  NAMES vulkan/vulkan.h
  HINTS
    ${_Vulkan_hint_include_search_paths}
  )

find_library(Vulkan_LIBRARY
  NAMES vulkan-1
  HINTS
    ${_Vulkan_hint_library_search_paths}
  )
find_program(Vulkan_GLSLC_EXECUTABLE
  NAMES glslc
  HINTS
    ${_Vulkan_hint_executable_search_paths}
  )
find_program(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE
  NAMES glslangValidator
  HINTS
    ${_Vulkan_hint_executable_search_paths}
  )
if(glslang IN_LIST Vulkan_FIND_COMPONENTS)
  if(NOT glslang-spirv IN_LIST Vulkan_FIND_COMPONENTS)
    list(APPEND Vulkan_FIND_COMPONENTS glslang-spirv)
  endif()
  if(NOT glslang-oglcompiler IN_LIST Vulkan_FIND_COMPONENTS)
    list(APPEND Vulkan_FIND_COMPONENTS glslang-oglcompiler)
  endif()
  if(NOT glslang-osdependent IN_LIST Vulkan_FIND_COMPONENTS)
    list(APPEND Vulkan_FIND_COMPONENTS glslang-osdependent)
  endif()
  if(NOT glslang-machineindependent IN_LIST Vulkan_FIND_COMPONENTS)
    list(APPEND Vulkan_FIND_COMPONENTS glslang-machineindependent)
  endif()
  if(NOT glslang-genericcodegen IN_LIST Vulkan_FIND_COMPONENTS)
    list(APPEND Vulkan_FIND_COMPONENTS glslang-genericcodegen)
  endif()
  find_library(Vulkan_glslang-spirv_LIBRARY
    NAMES SPIRV
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-oglcompiler_LIBRARY
    NAMES OGLCompiler
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-oglcompiler_DEBUG_LIBRARY
    NAMES OGLCompilerd
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-osdependent_LIBRARY
    NAMES OSDependent
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-osdependent_DEBUG_LIBRARY
    NAMES OSDependentd
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-machineindependent_LIBRARY
    NAMES MachineIndependent
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-machineindependent_DEBUG_LIBRARY
    NAMES MachineIndependentd
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-genericcodegen_LIBRARY
    NAMES GenericCodeGen
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang-genericcodegen_DEBUG_LIBRARY
    NAMES GenericCodeGend
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang_LIBRARY
    NAMES glslang
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
  find_library(Vulkan_glslang_DEBUG_LIBRARY
    NAMES glslangd
    HINTS
      ${_Vulkan_hint_library_search_paths}
    )
endif()
if(shaderc-combined IN_LIST Vulkan_FIND_COMPONENTS)
    find_library(Vulkan_shaderc-combined_LIBRARY
      NAMES shaderc_combined
      HINTS
      ${_Vulkan_hint_library_search_paths})
    find_library(Vulkan_shaderc-combined_DEBUG_LIBRARY
      NAMES shaderc_combinedd
      HINTS
      ${_Vulkan_hint_library_search_paths})
endif()

if(Vulkan_glslang-spirv_LIBRARY OR Vulkan_glslang-spirv_DEBUG_LIBRARY)
  set(Vulkan_glslang-spirv_FOUND TRUE)
else()
  set(Vulkan_glslang-spirv_FOUND FALSE)
endif()

if(Vulkan_glslang-oglcompiler_LIBRARY OR Vulkan_glslang-oglcompiler_DEBUG_LIBRARY)
  set(Vulkan_glslang-oglcompiler_FOUND TRUE)
else()
  set(Vulkan_glslang-oglcompiler_FOUND FALSE)
endif()

if(Vulkan_glslang-osdependent_LIBRARY OR Vulkan_glslang-osdependent_DEBUG_LIBRARY)
  set(Vulkan_glslang-osdependent_FOUND TRUE)
else()
  set(Vulkan_glslang-osdependent_FOUND FALSE)
endif()

if(Vulkan_glslang-machineindependent_LIBRARY OR Vulkan_glslang-machineindependent_DEBUG_LIBRARY)
  set(Vulkan_glslang-machineindependent_FOUND TRUE)
else()
  set(Vulkan_glslang-machineindependent_FOUND FALSE)
endif()

if(Vulkan_glslang-genericcodegen_LIBRARY OR Vulkan_glslang-genericcodegen_DEBUG_LIBRARY)
  set(Vulkan_glslang-genericcodegen_FOUND TRUE)
else()
  set(Vulkan_glslang-genericcodegen_FOUND FALSE)
endif()

# For Vulkan_glslang_FOUND to be really found, it requires all other Vulkan_glslang-XXX_FOUND to be found as well as they are dependent libraries
if((Vulkan_glslang_LIBRARY OR Vulkan_glslang_DEBUG_LIBRARY)
  AND (Vulkan_glslang-spirv_LIBRARY OR Vulkan_glslang-spirv_DEBUG_LIBRARY)
  AND (Vulkan_glslang-oglcompiler_LIBRARY OR Vulkan_glslang-oglcompiler_DEBUG_LIBRARY)
  AND (Vulkan_glslang-osdependent_LIBRARY OR Vulkan_glslang-osdependent_DEBUG_LIBRARY)
  AND (Vulkan_glslang-machineindependent_LIBRARY OR Vulkan_glslang-machineindependent_DEBUG_LIBRARY)
  AND (Vulkan_glslang-genericcodegen_LIBRARY OR Vulkan_glslang-genericcodegen_DEBUG_LIBRARY))
  set(Vulkan_glslang_FOUND TRUE)
else()
  set(Vulkan_glslang_FOUND FALSE)
endif()

if(Vulkan_shaderc-combined_LIBRARY OR Vulkan_shaderc-combined_DEBUG_LIBRARY)
  set(Vulkan_shaderc-combined_FOUND TRUE)
else()
  set(Vulkan_shaderc-combined_FOUND FALSE)
endif()

set(Vulkan_LIBRARIES ${Vulkan_LIBRARY})
set(Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR})

# detect version e.g 1.2.189
set(Vulkan_VERSION "")
if(Vulkan_INCLUDE_DIR)
  set(VULKAN_CORE_H ${Vulkan_INCLUDE_DIR}/vulkan/vulkan_core.h)
  if(EXISTS ${VULKAN_CORE_H})
    file(STRINGS  ${VULKAN_CORE_H} VulkanHeaderVersionLine REGEX "^#define VK_HEADER_VERSION ")
    string(REGEX MATCHALL "[0-9]+" VulkanHeaderVersion "${VulkanHeaderVersionLine}")
    file(STRINGS  ${VULKAN_CORE_H} VulkanHeaderVersionLine2 REGEX "^#define VK_HEADER_VERSION_COMPLETE ")
    string(REGEX MATCHALL "[0-9]+" VulkanHeaderVersion2 "${VulkanHeaderVersionLine2}")
    list(LENGTH VulkanHeaderVersion2 _len)
    #  versions >= 1.2.175 have an additional numbers in front of e.g. '0, 1, 2' instead of '1, 2'
    if(_len EQUAL 3)
        list(REMOVE_AT VulkanHeaderVersion2 0)
    endif()
    list(APPEND VulkanHeaderVersion2 ${VulkanHeaderVersion})
    list(JOIN VulkanHeaderVersion2 "." Vulkan_VERSION)
  endif()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(Vulkan
  REQUIRED_VARS
    Vulkan_LIBRARY
    Vulkan_INCLUDE_DIR
  VERSION_VAR
    Vulkan_VERSION
  HANDLE_COMPONENTS
)

mark_as_advanced(Vulkan_INCLUDE_DIR
                 Vulkan_LIBRARY
                 Vulkan_GLSLC_EXECUTABLE
                 Vulkan_GLSLANG_VALIDATOR_EXECUTABLE
                 Vulkan_glslang-spirv_LIBRARY
                 Vulkan_glslang-spirv_DEBUG_LIBRARY
                 Vulkan_glslang-oglcompiler_LIBRARY
                 Vulkan_glslang-oglcompiler_DEBUG_LIBRARY
                 Vulkan_glslang-osdependent_LIBRARY
                 Vulkan_glslang-osdependent_DEBUG_LIBRARY
                 Vulkan_glslang-machineindependent_LIBRARY
                 Vulkan_glslang-machineindependent_DEBUG_LIBRARY
                 Vulkan_glslang-genericcodegen_LIBRARY
                 Vulkan_glslang-genericcodegen_DEBUG_LIBRARY
                 Vulkan_glslang_LIBRARY
                 Vulkan_glslang_DEBUG_LIBRARY
                 Vulkan_shaderc-combined_DEBUG_LIBRARY
                 Vulkan_shaderc-combined_RELEASE_LIBRARY
)

if(Vulkan_FOUND AND NOT TARGET Vulkan::Vulkan)
  add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
  set_target_properties(Vulkan::Vulkan PROPERTIES
    IMPORTED_LOCATION "${Vulkan_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
endif()

if(Vulkan_FOUND AND NOT TARGET Vulkan::Headers)
  add_library(Vulkan::Headers INTERFACE IMPORTED)
  set_target_properties(Vulkan::Headers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
endif()

if(Vulkan_FOUND AND Vulkan_GLSLC_EXECUTABLE AND NOT TARGET Vulkan::glslc)
  add_executable(Vulkan::glslc IMPORTED)
  set_property(TARGET Vulkan::glslc PROPERTY IMPORTED_LOCATION "${Vulkan_GLSLC_EXECUTABLE}")
endif()

if(Vulkan_FOUND AND Vulkan_GLSLANG_VALIDATOR_EXECUTABLE AND NOT TARGET Vulkan::glslangValidator)
  add_executable(Vulkan::glslangValidator IMPORTED)
  set_property(TARGET Vulkan::glslangValidator PROPERTY IMPORTED_LOCATION "${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}")
endif()

if(Vulkan_FOUND)
  if((Vulkan_glslang-spirv_LIBRARY OR Vulkan_glslang-spirv_DEBUG_LIBRARY) AND NOT TARGET Vulkan::glslang-spirv)
    add_library(Vulkan::glslang-spirv STATIC IMPORTED)
    set_property(TARGET Vulkan::glslang-spirv
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_glslang-spirv_LIBRARY)
      set_property(TARGET Vulkan::glslang-spirv APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::glslang-spirv
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_glslang-spirv_LIBRARY}")
    endif()
    if(Vulkan_glslang-spirv_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::glslang-spirv APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::glslang-spirv
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_glslang-spirv_DEBUG_LIBRARY}")
    endif()
  endif()

  if((Vulkan_glslang-oglcompiler_LIBRARY OR Vulkan_glslang-oglcompiler_DEBUG_LIBRARY) AND NOT TARGET Vulkan::glslang-oglcompiler)
    add_library(Vulkan::glslang-oglcompiler STATIC IMPORTED)
    set_property(TARGET Vulkan::glslang-oglcompiler
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_glslang-oglcompiler_LIBRARY)
      set_property(TARGET Vulkan::glslang-oglcompiler APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::glslang-oglcompiler
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_glslang-oglcompiler_LIBRARY}")
    endif()
    if(Vulkan_glslang-oglcompiler_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::glslang-oglcompiler APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::glslang-oglcompiler
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_glslang-oglcompiler_DEBUG_LIBRARY}")
    endif()
  endif()

  if((Vulkan_glslang-osdependent_LIBRARY OR Vulkan_glslang-osdependent_DEBUG_LIBRARY) AND NOT TARGET Vulkan::glslang-osdependent)
    add_library(Vulkan::glslang-osdependent STATIC IMPORTED)
    set_property(TARGET Vulkan::glslang-osdependent
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_glslang-osdependent_LIBRARY)
      set_property(TARGET Vulkan::glslang-osdependent APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::glslang-osdependent
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_glslang-osdependent_LIBRARY}")
    endif()
    if(Vulkan_glslang-osdependent_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::glslang-osdependent APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::glslang-osdependent
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_glslang-osdependent_DEBUG_LIBRARY}")
    endif()
  endif()

  if((Vulkan_glslang-machineindependent_LIBRARY OR Vulkan_glslang-machineindependent_DEBUG_LIBRARY) AND NOT TARGET Vulkan::glslang-machineindependent)
    add_library(Vulkan::glslang-machineindependent STATIC IMPORTED)
    set_property(TARGET Vulkan::glslang-machineindependent
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_glslang-machineindependent_LIBRARY)
      set_property(TARGET Vulkan::glslang-machineindependent APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::glslang-machineindependent
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_glslang-machineindependent_LIBRARY}")
    endif()
    if(Vulkan_glslang-machineindependent_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::glslang-machineindependent APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::glslang-machineindependent
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_glslang-machineindependent_DEBUG_LIBRARY}")
    endif()
  endif()

  if((Vulkan_glslang-genericcodegen_LIBRARY OR Vulkan_glslang-genericcodegen_DEBUG_LIBRARY) AND NOT TARGET Vulkan::glslang-genericcodegen)
    add_library(Vulkan::glslang-genericcodegen STATIC IMPORTED)
    set_property(TARGET Vulkan::glslang-genericcodegen
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_glslang-genericcodegen_LIBRARY)
      set_property(TARGET Vulkan::glslang-genericcodegen APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::glslang-genericcodegen
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_glslang-genericcodegen_LIBRARY}")
    endif()
    if(Vulkan_glslang-genericcodegen_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::glslang-genericcodegen APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::glslang-genericcodegen
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_glslang-genericcodegen_DEBUG_LIBRARY}")
    endif()
  endif()

  if((Vulkan_glslang_LIBRARY OR Vulkan_glslang_DEBUG_LIBRARY)
      AND TARGET Vulkan::glslang-spirv
      AND TARGET Vulkan::glslang-oglcompiler
      AND TARGET Vulkan::glslang-osdependent
      AND TARGET Vulkan::glslang-machineindependent
      AND TARGET Vulkan::glslang-genericcodegen
      AND NOT TARGET Vulkan::glslang)
    add_library(Vulkan::glslang STATIC IMPORTED)
    set_property(TARGET Vulkan::glslang
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_glslang_LIBRARY)
      set_property(TARGET Vulkan::glslang APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::glslang
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_glslang_LIBRARY}")
    endif()
    if(Vulkan_glslang_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::glslang APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::glslang
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_glslang_DEBUG_LIBRARY}")
    endif()
    target_link_libraries(Vulkan::glslang
      INTERFACE
        Vulkan::glslang-spirv
        Vulkan::glslang-oglcompiler
        Vulkan::glslang-osdependent
        Vulkan::glslang-machineindependent
        Vulkan::glslang-genericcodegen
    )
  endif()

  if((Vulkan_shaderc-combined_LIBRARY OR Vulkan_shaderc-combined_DEBUG_LIBRARY) AND NOT TARGET Vulkan::shaderc-combined)
    add_library(Vulkan::shaderc-combined STATIC IMPORTED)
    set_property(TARGET Vulkan::shaderc-combined
      PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
    if(Vulkan_shaderc-combined_LIBRARY)
      set_property(TARGET Vulkan::shaderc-combined APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Release)
      set_property(TARGET Vulkan::shaderc-combined
        PROPERTY
          IMPORTED_LOCATION_RELEASE "${Vulkan_shaderc-combined_LIBRARY}")
    endif()
    if(Vulkan_shaderc-combined_DEBUG_LIBRARY)
      set_property(TARGET Vulkan::shaderc-combined APPEND
        PROPERTY
          IMPORTED_CONFIGURATIONS Debug)
      set_property(TARGET Vulkan::shaderc-combined
        PROPERTY
          IMPORTED_LOCATION_DEBUG "${Vulkan_shaderc-combined_DEBUG_LIBRARY}")
    endif()

    if(UNIX)
      find_package(Threads REQUIRED)
      target_link_libraries(Vulkan::shaderc-combined
        INTERFACE
          Threads::Threads)
    endif()
  endif()
endif()

unset(_Vulkan_hint_include_search_paths)
unset(_Vulkan_hint_executable_search_paths)
unset(_Vulkan_hint_library_search_paths)
