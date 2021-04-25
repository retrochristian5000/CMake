
cmake_policy(SET CMP0123 OLD)

foreach(VAR a b c)
endforeach()

if (NOT DEFINED VAR OR NOT VAR STREQUAL "")
  message(SEND_ERROR "Variable 'VAR' not defined or wrong value.")
endif()


set (VAR2 OLD)

foreach(VAR2 a b c)
endforeach()

if (NOT DEFINED VAR2 OR NOT VAR2 STREQUAL "OLD")
  message(SEND_ERROR "Variable 'VAR2' not defined or wrong value.")
endif()


set (VAR3 OLD CACHE STRING "")

foreach(VAR3 a b c)
endforeach()

# a normal variable is defined, holding cache variable value
unset(VAR3 CACHE)
if (NOT DEFINED VAR3 OR NOT VAR3 STREQUAL "OLD")
  message(SEND_ERROR "Variable 'VAR3' not defined or wrong value.")
endif()
