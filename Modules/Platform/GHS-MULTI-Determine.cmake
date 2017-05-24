# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#Setup Green Hills MULTI specific compilation information


set(GHS_TARGET_PLATFORM "integrity" CACHE STRING "GHS target platform")

#set and validate GHS_OS_ROOT
set(GHS_OS_ROOT "C:/ghs" CACHE PATH "GHS platform OS root directory")
mark_as_advanced(GHS_OS_ROOT)
if (NOT IS_ABSOLUTE ${GHS_OS_ROOT})
  message( FATAL_ERROR "GHS_OS_ROOT \"${GHS_OS_ROOT}\" is not absolute path" )
endif ()

#set GHS_OS_DIR
if ( NOT GHS_OS_DIR )
  if (EXISTS ${GHS_OS_ROOT})

    #get all directories in root directory
    FILE(GLOB GHS_CANDIDATE_INT_DIRS RELATIVE ${GHS_OS_ROOT} ${GHS_OS_ROOT}/*)
    FILE(GLOB GHS_CANDIDATE_INT_FILES
      LIST_DIRECTORIES false RELATIVE ${GHS_OS_ROOT} ${GHS_OS_ROOT}/*)
    list(REMOVE_ITEM GHS_CANDIDATE_INT_DIRS ${GHS_CANDIDATE_INT_FILES})

    if (GHS_TARGET_PLATFORM STREQUAL "integrity")
      list(FILTER GHS_CANDIDATE_INT_DIRS INCLUDE REGEX "int[0-9][0-9][0-9][0-9].*")
      set(RTOS "" )
    elseif (GHS_TARGET_PLATFORM STREQUAL "integrity178")
      list(FILTER GHS_CANDIDATE_INT_DIRS INCLUDE REGEX "int178_*")
      set(RTOS "/rtos" )
    elseif (GHS_TARGET_PLATFORM STREQUAL "integrityIOT")
      list(FILTER GHS_CANDIDATE_INT_DIRS INCLUDE REGEX "int-mz-[0-9][0-9][0-9][0-9].*")
      set(RTOS "/rtos" )
    endif ()

    if (GHS_CANDIDATE_INT_DIRS)
      list(SORT GHS_CANDIDATE_INT_DIRS)
      list(GET GHS_CANDIDATE_INT_DIRS -1 GHS_INT_DIRECTORY)
      string(CONCAT GHS_OS_LOCATION ${GHS_OS_ROOT} "/" ${GHS_INT_DIRECTORY} ${RTOS})
    endif()

    set(GHS_OS_DIR "${GHS_OS_LOCATION}" CACHE PATH "GHS platform OS directory")
  endif ()
endif ()

set(GHS_BSP_NAME "simarm" CACHE STRING "BSP name")
set(GHS_CUSTOMIZATION "" CACHE FILEPATH "optional GHS customization")
mark_as_advanced(GHS_CUSTOMIZATION)
set(GHS_GPJ_MACROS "" CACHE STRING "optional GHS macros generated in the .gpjs for legacy reasons")
mark_as_advanced(GHS_GPJ_MACROS)

#set and validate GHS_TOOLSET_ROOT
set(GHS_TOOLSET_ROOT "C:/ghs" CACHE PATH "GHS toolset location.")
mark_as_advanced(GHS_TOOLSET_ROOT)
if (NOT IS_ABSOLUTE ${GHS_TOOLSET_ROOT})
  message( FATAL_ERROR "GHS_TOOLSET_ROOT \"${GHS_TOOLSET_ROOT}\" is not absolute path" )
endif ()
