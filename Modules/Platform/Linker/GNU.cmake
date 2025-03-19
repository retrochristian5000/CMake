# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.


# This module is shared by multiple languages; use include blocker.
include_guard()

# WHOLE_ARCHIVE Feature for LINK_LIBRARY generator expression
## check linker capabilities
function(__cmake_set_whole_archive_feature __linker)
  unset(__lang)
  if(ARGC EQUAL "2")
    set(__lang "${ARGV1}_")
  endif()

  if(NOT __linker)
    set(CMAKE_${__lang}LINKER_PUSHPOP_STATE_SUPPORTED FALSE)
  endif()

  if(NOT DEFINED CMAKE_${__lang}LINKER_PUSHPOP_STATE_SUPPORTED)
    # launch linker to check if push_state/pop_state options are supported
    execute_process(COMMAND "${__linker}" --push-state --pop-state
                    OUTPUT_VARIABLE __linker_log
                    ERROR_VARIABLE __linker_log)
    if(__linker_log MATCHES "--push-state" OR __linker_log MATCHES "--pop-state")
      set(linker_pushpop_state_supported FALSE)
    else()
      set(linker_pushpop_state_supported TRUE)
    endif()
    set(CMAKE_${__lang}LINKER_PUSHPOP_STATE_SUPPORTED ${linker_pushpop_state_supported} PARENT_SCOPE)
  endif()
  ## WHOLE_ARCHIVE: Force loading all members of an archive
  if(linker_pushpop_state_supported)
    set(CMAKE_${__lang}LINK_LIBRARY_USING_WHOLE_ARCHIVE "LINKER:--push-state,--whole-archive"
                                                        "<LINK_ITEM>"
                                                        "LINKER:--pop-state" PARENT_SCOPE)
  else()
    set(CMAKE_${__lang}LINK_LIBRARY_USING_WHOLE_ARCHIVE "LINKER:--whole-archive"
                                                        "<LINK_ITEM>"
                                                        "LINKER:--no-whole-archive" PARENT_SCOPE)
  endif()
  set(CMAKE_${__lang}LINK_LIBRARY_USING_WHOLE_ARCHIVE_SUPPORTED TRUE PARENT_SCOPE)
  set(CMAKE_${__lang}LINK_LIBRARY_WHOLE_ARCHIVE_ATTRIBUTES LIBRARY_TYPE=STATIC DEDUPLICATION=YES OVERRIDE=DEFAULT PARENT_SCOPE)
endfunction()


## Configure system linker
__cmake_set_whole_archive_feature("${CMAKE_LINKER}")
