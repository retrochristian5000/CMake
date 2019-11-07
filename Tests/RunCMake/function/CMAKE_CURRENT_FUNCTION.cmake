if(DEFINED CMAKE_CURRENT_FUNCTION)
  message(SEND_ERROR "`CMAKE_CURRENT_FUNCTION` is not expected to be set here")
endif()

function(bar)
  if(NOT CMAKE_CURRENT_FUNCTION STREQUAL "bar")
    message(SEND_ERROR "Bad value of `CMAKE_CURRENT_FUNCTION`")
  endif()
endfunction()

function(foo)
  if(NOT CMAKE_CURRENT_FUNCTION STREQUAL "foo")
    message(SEND_ERROR "Bad value of `CMAKE_CURRENT_FUNCTION`")
  endif()
  bar()
  include("${CMAKE_CURRENT_LIST_DIR}/CMAKE_CURRENT_FUNCTION-include.cmake")
  add_subdirectory(subdir)
endfunction()

foo()

if(DEFINED CMAKE_CURRENT_FUNCTION)
  message(SEND_ERROR "`CMAKE_CURRENT_FUNCTION` is not expected to be set here")
endif()

macro(dummy)
  if(NOT CMAKE_CURRENT_FUNCTION STREQUAL "calling_macro")
    message(SEND_ERROR "Bad value of `CMAKE_CURRENT_FUNCTION`")
  endif()
endmacro()

function(calling_macro)
  dummy()
endfunction()

calling_macro()
