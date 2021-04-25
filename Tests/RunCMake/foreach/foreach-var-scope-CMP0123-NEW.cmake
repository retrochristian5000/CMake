
cmake_policy(SET CMP0123 NEW)

foreach(VAR a b c)
endforeach()

if (DEFINED VAR)
  message(SEND_ERROR "Variable 'VAR' unexpectedly defined.")
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

# check that only cache variable is defined
set(OLD_VALUE "${VAR3}")
unset(VAR3 CACHE)
if (DEFINED VAR3 OR NOT OLD_VALUE STREQUAL "OLD")
  message(SEND_ERROR "Variable 'VAR3' wrongly defined or wrong value.")
endif()
