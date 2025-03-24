set_variable(FOO VALUES)
set_variable(BAR VALUES "bar")
set_variable(BAZ VALUES "baz")
set_variable(BOO VALUES "boo")

function(both_scopes)
    set_variable(FOO SCOPE LOCAL PARENT VALUES "foo")
    set_variable(BAR SCOPE LOCAL PARENT VALUES "")
    set_variable(BAZ SCOPE LOCAL PARENT VALUES)
    unset_variable(BOO SCOPE LOCAL PARENT)

    if(NOT DEFINED FOO)
      message(FATAL_ERROR "in function: FOO not defined")
    elseif(NOT "${FOO}" STREQUAL "foo")
      message(FATAL_ERROR "in function: FOO should be \"foo\", not \"${FOO}\"")
    endif()

    if(NOT DEFINED BAR)
      message(FATAL_ERROR "in function: BAR not defined")
    elseif(NOT "${BAR}" STREQUAL "")
      message(FATAL_ERROR "in function: BAR should be \"\", not \"${BAR}\"")
    endif()

    if(NOT DEFINED BAZ)
      message(FATAL_ERROR "in function: BAZ not defined")
    elseif(NOT "${BAZ}" STREQUAL "")
      message(FATAL_ERROR "in function: BAZ should be \"\", not \"${BAZ}\"")
    endif()

    if(DEFINED BOO)
      message(FATAL_ERROR "in function: BOO defined")
    endif()
endfunction()

both_scopes()


if(NOT DEFINED FOO)
  message(FATAL_ERROR "FOO not defined")
elseif(NOT "${FOO}" STREQUAL "foo")
  message(FATAL_ERROR "FOO should be \"foo\", not \"${FOO}\"")
endif()

if(NOT DEFINED BAR)
  message(FATAL_ERROR "BAR not defined")
elseif(NOT "${BAR}" STREQUAL "")
  message(FATAL_ERROR "BAR should be an empty string, not \"${BAR}\"")
endif()

if(NOT DEFINED BAZ)
  message(FATAL_ERROR "BAZ not defined")
elseif(NOT "${BAZ}" STREQUAL "")
  message(FATAL_ERROR "BAZ should be an empty string, not \"${BAZ}\"")
endif()

if(DEFINED BOO)
  message(FATAL_ERROR "BOO defined")
endif()
